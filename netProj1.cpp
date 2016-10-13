#include <iostream>
#include <random>


//int delay_time_total = SIF + ACK + DIF + backoff + data_frames;

int y = 0;
int frame_size[] = {50, 100, 200, 300, 400, 500}; // 300, 400, 500
int cw_range[] = {4, 8, 16, 32, 64, 128, 512, 1024};
int data_frame_size = 1500; //bytes
double seconds_to_microseconds = 0.00001;
int slot_duration = 20; //microseconds
double fifty_frames = 0.36;
double hundred_frames = 0.66;
double twohundred_frames = 1.26 ;
double threehundred_frames = 1.86;
double fourhundred_frames = 2.46;
double fivehundred_frames = 3.06;
double time_add[] = {fifty_frames, hundred_frames, twohundred_frames, threehundred_frames, fourhundred_frames, fivehundred_frames};

// A calculation of (bytes/frame)(frames/second)(seconds/microsecond)(microseconds/slot)
//  giving total bytes per slot
double bytes_per_slot = data_frame_size * frame_size[y] * seconds_to_microseconds * slot_duration;
int sifs_duration = 10; // microseconds
int difs_duration = 40; //microseconds
int ack = 30; //bytes
int ack_time = 2 * slot_duration;
int tx_rate = 6; //Mbps
time_t simulation_time = 10000; // microseconds
int collisions = 0;
int delay_time = 0; //
int backoff_diff = 0;
int max_range = 4;
double total_time = 0;
double micro_to_milisecond_conversion = (1/1000);
int node_a_successful_sends = 0;
int node_c_successful_sends = 0;
int backoff_slots_holder_A = 0;
int backoff_slots_holder_C = 0;

double slots_per_frame = (data_frame_size / bytes_per_slot);

void compute_max_range_value(){
    max_range = cw_range[collisions];
    if(collisions >= 7) {
        max_range = cw_range[7];
    }
}

int random_value_generator(){
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, max_range);
    int random_number = uni(rng);
    return random_number;
};

class Sender {
public:
    Sender(){}
    double total_time = 0;
    //double successful_sends = 0;
    double frame_size = slots_per_frame;
    //int backoff_slots = random_value_generator();
    int compute_backoff_slots_A();
    int compute_backoff_slots_C();
private:
};

int Sender::compute_backoff_slots_A() {
    backoff_slots_holder_A = 0;
    int backoff_slots = random_value_generator();
    backoff_slots_holder_A += backoff_slots;
    return backoff_slots;
}
int Sender::compute_backoff_slots_C() {
    backoff_slots_holder_C = 0;
    int backoff_slots = random_value_generator();
    backoff_slots_holder_C += backoff_slots;
    return backoff_slots;
}

double compare_backoff_delay(Sender one, Sender two){
    double b1 = one.frame_size + one.compute_backoff_slots_A();
    double b2 = two.frame_size + two.compute_backoff_slots_C();
    double difference = std::abs(b1-b2);
    return difference;
}



void send_successful(Sender one, Sender two){
    if(backoff_slots_holder_A < backoff_slots_holder_C){
        node_a_successful_sends++;
    } else {
        node_c_successful_sends++;
    }
}
double collision_duration(Sender one, Sender two){
    double col_dur = slots_per_frame - compare_backoff_delay(one, two);
    return col_dur;
}
void packet_collision(Sender one, Sender two){
    collisions++;
    compute_max_range_value();
}

// Still need to figure out the timers.
void run(Sender one, Sender two){
    total_time += difs_duration;
    if(compare_backoff_delay(one, two) < slots_per_frame) {
        total_time += (20 + (collision_duration(one, two) * 20)) * 1/1000;
        packet_collision(one, two);
    } else {
        total_time += time_add[y];
        send_successful(one, two);
    }

}
void reset(){
    node_a_successful_sends = 0;
    node_c_successful_sends = 0;
    backoff_slots_holder_A = 0;
    backoff_slots_holder_C = 0;
    collisions = 0;
}

int main() {
    for(y = 0; y < frame_size[y]; y++){
        Sender a, b;

        do {
            run(a,b);
        } while(total_time < simulation_time);
        std::cout << "Run with " << frame_size[y] << " frames per second." << std::endl;
        std::cout << "Successful Sends from Node A: " << node_a_successful_sends << std::endl;
        std::cout << "Successful Sends from Node C: " << node_c_successful_sends << std::endl;
        std::cout << "Total Number of Collisions: " << collisions << std::endl;
        std::cout << " " << std::endl;
        //reset();
    }


    return 0;
}

// bytes per slot is throughput

// how many slots does one frame take

// convert to microseconds
