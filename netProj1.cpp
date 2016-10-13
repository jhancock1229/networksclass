#include <iostream>
#include <random>


//int delay_time_total = SIF + ACK + DIF + backoff + data_frames;


int frame_size[] = {50, 100, 200, 300, 400, 500}; // 300, 400, 500
int cw_range[] = {4, 8, 16, 32, 64, 128, 512, 1024};
int data_frame_size = 1500; //bytes
double seconds_to_microseconds = 0.00001;
int slot_duration = 20; //microseconds

// A calculation of (bytes/frame)(frames/second)(seconds/microsecond)(microseconds/slot)
//  giving total bytes per slot
double bytes_per_slot = data_frame_size * frame_size[0] * seconds_to_microseconds * slot_duration;
int sifs_duration = 10; // microseconds
int difs_duration = 40; //microseconds
int ack = 30; //bytes
int tx_rate = 6; //Mbps
time_t simulation_time = 10; // seconds
int collisions = 0;
int delay_time = 0; //
int backoff_diff = 0;
int max_range = 4;



double slots_per_frame = (data_frame_size / bytes_per_slot);

void compute_max_range_value(){
    max_range = max_range * (2 * collisions);
}

int random_value_generator(){
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_int_distribution<int> uni(0, max_range);
    int random_number = uni(rng);
    return random_number;
};

double compare_backoff_delay(Sender one, Sender two){
    double b1 = one.frame_size + one.compute_backoff_slots();
    double b2 = two.frame_size + two.compute_backoff_slots();
    double difference = abs(b1-b2);
    return difference;
}

class Sender {
public:
    Sender() : delay(0), backoff_slots(0) {}
    double total_time = 0;
    double frame_size = slots_per_frame;

    int compute_backoff_slots(){
        int backoff_slots = random_value_generator();
        return backoff_slots;
    }
private:
    int backoff_slots;
    int delay;
};


void send_successful(Sender one, Sender two){
    if(one.compute_backoff_slots() < two.compute_backoff_slots()){
        one.total_time += ((2* sifs_duration) + ack + (bytes_per_slot *slot_duration));
    } else {
        two.total_time += ((2* sifs_duration) + ack + (bytes_per_slot *slot_duration));
    }
}
double collision_duration(Sender one, Sender two){
    double col_dur = slots_per_frame - compare_backoff_delay(one, two);
    return col_dur;
}
void packet_collision(Sender one, Sender two){
    collisions++;
    one.total_time += sifs_duration + collision_duration(one, two);
    two.total_time += sifs_duration + collision_duration(one, two);
    compute_max_range_value();
}

// Still need to figure out the timers.
void run(Sender one, Sender two){
    one.total_time += difs_duration;
    two.total_time += difs_duration;
    if(compare_backoff_delay(one, two) < slots_per_frame) {
        packet_collision(one, two);
        run(one, two);
    } else {
        send_successful(one, two);
        run(one, two);
    }

}


int main() {
    time_t endwait;
    time_t start = time(NULL);
    endwait = start + simulation_time;

    Sender a, b;

    do {
        run(a,b);
    } while(start < endwait);
    
    return 0;
}

// bytes per slot is throughput

// how many slots does one frame take