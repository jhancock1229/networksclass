#include <iostream>
#include <random>

int cw_range[] = {4, 8, 16, 32, 64, 128, 512, 1024};
int data_frame_size = 1500; //bytes
int slot_duration = 20; //microseconds
int SIFS_duration = 10; // microseconds
int comm_window_min = 4; //slots
int comm_window_max = 1024; //slots
int ack = 30; //bytes
int rts = ack;
int cts = rts;
int DIFS_duration = 40; //microseconds
int tx_rate = 6; //Mbps
int simulation_time = 10; // seconds
int collisions = 0;
int delay_time = 0; //
int backoff_diff = 0;
bool is_collision = false;



int frame_size[] = {50, 100, 200, 300, 400, 500}; // 300, 400, 500

// ^^convert all values to microseconds^^^
//



class Sender {
public:
    Sender() : delay(0), backoff(0) {}
    int compute_backoff() {
        int max_range = cw_range[collisions] -1;
        std::random_device rd;
        std::mt19937 rng(rd());
        std::uniform_int_distribution<int> uni(0, 4);
        backoff = uni(rng);
        return backoff;
    }

    int get_rts_time(){
        return backoff;
    }



private:
    int backoff;
    int delay;
};
class Computations{
public:
    Computations(int fs) {}


    bool compare_backoffs(Sender one, Sender two){
        int b1 = one.get_rts_time();
        int b2 = two.get_rts_time();
        int difference = abs(b1-b2);
        if(difference < rts) {
            collisions++;
            backoff_diff += difference;
            is_collision = true;
            return is_collision;
        }
        return true;
    }
    void convert(int fs){
        for(int x = 0; x < 6; x++){
            // Converts cw slots into microseconds
            cw_range[x] = frame_size[x] * slot_duration;
            // Converts data_frame_size into bytes per microsecond
            data_frame_size = (data_frame_size * frame_size[x] * (slot_duration * 0.000001));
        }
    }
private:
    int frame_size_index;
};
void delay(){
    delay_time += (rts + cts + ack + DIFS_duration + 3*SIFS_duration + data_frame_size);
}

void collision_delay(){
    delay_time += (2*rts - backoff_diff);// (2*RTS - difference)
}

int main() {
    Sender a, b;
    for(int i=0; i<6 ;i++)
    {
        // compute bytes per second here
        Computations compute(i);
        compute.convert(i);
        for(int j = 0; j < frame_size[i]; j++)
        {
            a.compute_backoff();
            b.compute_backoff();
            compute.compare_backoffs(a, b);
            if(is_collision == true){
                collision_delay();
                delay();

            } else {
                delay();
            }
        }

    }


    // cout << number of collisions
    // cout << Throughput
    // cout << delay
    // cout << fairness index (which is throughput for A divided by throughput for B)
    return 0;
}

// bytes per slot is throughput

// how many slots does one frame take