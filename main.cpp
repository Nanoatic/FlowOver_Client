#include "includes.h"
#include "constants_and_definitions.h"
#include "helper_class.h"
#include "sync_queue.h"

atomic<bool> connected {true};
atomic_int socket_;
sync_queue<vector<byte>> message_dispatcher_queue={};
sync_queue<vector<byte>> message_poller_queue={};
atomic_int sent{0};

// Helpers Section

void cout_with_sync(const std::string &msg) {
    stringstream stringstream_;
    stringstream_ << msg << endl;
    cout << stringstream_.str();
}




class keep_alive  {
public:
    keep_alive()= default;

    void init() ;

    void run() ;

    void shutdown() ;

    void start_ka() {
        thread=std::thread(&keep_alive::run, this);

    }

    std::thread thread;
    std::atomic<bool> running{true};

};
void keep_alive::init() {
    cout_with_sync("Keep Alive started");
}

void keep_alive::run() {
    init();
    while (running) {
        sleep(KEEP_ALIVE);
        vector<byte> array(LENGTH_PADDING_MANDATORY);
        message_dispatcher_queue.push(array);
    }
}

void keep_alive::shutdown() {
    cout_with_sync("Keep Alive shutdown");
    running= false;
    thread.join();
}



class message_dispatcher{
public:
    message_dispatcher() = default;

    void init() ;

    void run() ;

    void shutdown() ;

    void start_md() {
        thread=std::thread(&message_dispatcher::run, this);

    }

    std::thread thread;
    std::atomic<bool> running{true};

};

class message_poller  {
public:
    message_poller() = default;

    void init() ;

    void run() ;

    void shutdown() ;

    void start_mp() {
        thread_b=thread(&message_poller::run, this);

    }

    thread thread_b;
    atomic<bool> running{true};
};



void message_poller::init() {
    cout_with_sync("Message Poller started");
}

void message_poller::run() {
    init();
    while(running) {
        unsigned short size;
        helper_class::ReadXBytes(socket_, 2, &size);
        byte buffer[size];

        /* if (read(parent->client_socket_, buffer, MAX_BUFFER_SIZE) < 0) {

             parent->running = false;
             running = false;
             return;
         }*/
        memcpy(buffer,&size,sizeof(unsigned short));
        helper_class::ReadXBytes(socket_, size-2, buffer+2);
        message_poller_queue.push(helper_class::buffer_to_array(buffer,size));


    }

}

void message_poller::shutdown() {
    cout_with_sync("Message Poller shutdown");
    running = false;
    close(socket_);
    thread_b.join();
}


void message_dispatcher::init() {
    cout_with_sync("Message Dispatcher started");
}

void message_dispatcher::run() {
    init();
    while(running) {
        vector<byte> buf;
        message_dispatcher_queue.pop(buf);
        try {




                helper_class::send_all(socket_, buf.data(), buf.size());


        } catch (const std::exception &e) {

            running = false;
        }


    }


}

void message_dispatcher::shutdown() {
    cout_with_sync("Message Dispatcher shutdown");
    running = false;
    thread.join();
}


void proccess() {
    std::vector<byte> buf;

        message_poller_queue.pop(buf);
        if (buf[ID_LOCATION] == APP_ID) {

            switch (buf[INS_LOCATION]) {
                case INS_MULTIPLY_AND_SUM_R_X_C: {
                    float sum = 0;
                    unsigned short *sym=new  unsigned short;
                    memcpy(sym,buf.data()+MATRIX_MULTIPLICATION_LENGTH_LOCATION,sizeof(short));
                    int length = *sym * sizeof(double) ;
                    delete sym;
                    std::cout<<  "length :"<<length << endl;

                    int j = MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION + length;
                    for (int i = MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION;
                         i < MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION + length;
                         i+= sizeof(double)) {
                        float row = helper_class::array_to_elem<double>(buf.data(),i);
                        float col = helper_class::array_to_elem<double>(buf.data(),j);
                        sum += row * col;
                        if(i==MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION){
                            helper_class::display_info_syncronized_ln(to_string(row)+"*"+to_string(col));
                        }

                        j+= sizeof(double);
                    }
                    unsigned short  size1 = LENGTH_PADDING + sizeof(double);
                    vector<byte> send_buf(size1);
                    memcpy(send_buf.data(),&size1, sizeof(size1));
                    send_buf[ID_LOCATION] = APP_ID;
                    send_buf[INS_LOCATION] = INS_MULTIPLY_AND_SUM_R_X_C;
                    send_buf[TAG_LOCATION] = buf[TAG_LOCATION];
                    send_buf[ROW_INDEX_LOCATION]=buf[ROW_INDEX_LOCATION];
                    send_buf[ROW_INDEX_LOCATION+1]=buf[ROW_INDEX_LOCATION+1];
                    send_buf[COLUMN_INDEX_LOCATION]=buf[COLUMN_INDEX_LOCATION];
                    send_buf[COLUMN_INDEX_LOCATION+1]=buf[COLUMN_INDEX_LOCATION+1];
                    send_buf[MATRIX_MULTIPLICATION_LENGTH_LOCATION]= buf[MATRIX_MULTIPLICATION_LENGTH_LOCATION];
                    usleep(500);
                    ++sent;
                    union {
                        double res;
                        byte  buffer [8];
                    }converter;
                    converter.res=sum;
                    memcpy(send_buf.data()+MATRIX_MULTIPLICATION_BEGIN_OF_DATA_LOCATION, &converter.buffer , sizeof(double));
                    message_dispatcher_queue.push(send_buf);

                }
                    break;
                default:
                    break;

            }

        }
helper_class::display_info_syncronized_ln("sent: "+to_string(sent));


}

int connection_init() {
    int portno;
    sockaddr_in serv_addr{};
    hostent *server;


    portno = htons(SERVER_PORT);
    socket_ = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_ < 0) {
        std::cerr << "ERROR opening socket \n\n";
        return -1;
    }
    server = gethostbyname(SERVER_IP);
    if (server == nullptr) {
        std::cerr << "ERROR, no such host\n";
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr,
          (char *) &serv_addr.sin_addr.s_addr,
          static_cast<size_t>(server->h_length));
    serv_addr.sin_port = static_cast<in_port_t>(portno);
    in_addr ipAddr = serv_addr.sin_addr;
    char str[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &ipAddr, str, INET_ADDRSTRLEN);
    timeval t{};
    t.tv_sec = 0;
    t.tv_usec = 0;
    setsockopt(socket_, SOL_SOCKET, SO_RCVTIMEO, (&t),sizeof(t));
    std::cout << "Attempting to connect to server @ " << str << " ... " << std::endl;
    if (connect(socket_, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "ERROR connecting\n ";
        return -1;
    }
    std::cout << "Connected " << std::endl;
    return 0;
}
class interrupt_exception : public std::exception
{
public:
    explicit interrupt_exception(int s) : S(s) {}
    int S;
};


void sig_to_exception(int s)
{
    throw interrupt_exception(s);
}
int main() {

    struct sigaction sigIntHandler{};
    sigIntHandler.sa_handler = sig_to_exception;
    sigemptyset(&sigIntHandler.sa_mask);
    sigIntHandler.sa_flags = 0;
    sigaction(SIGINT, &sigIntHandler, nullptr);
    sigaction(SIGKILL, &sigIntHandler, nullptr);
    sigaction(SIGTERM, &sigIntHandler, nullptr);
    sigaction(SIGTERM, &sigIntHandler, nullptr);
    //signal(SIGPIPE, SIG_IGN);
    if (connection_init() == -1)
        return -1;
    message_dispatcher md;
    message_poller mp;
    keep_alive ka;
    md.start_md();
    mp.start_mp();
    //ka.start_ka();

    try {


    while (connected) {
        proccess();
    }

    ka.shutdown();
    mp.shutdown();
    md.shutdown();

    return 0;
    }catch (interrupt_exception & e){
        ka.shutdown();
        mp.shutdown();
        md.shutdown();
        close(socket_);
    }
}
