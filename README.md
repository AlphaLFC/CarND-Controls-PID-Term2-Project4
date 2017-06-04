# CarND-Controls-PID
This is my implement of a PID controller for CarND.

---

## Dependencies

* cmake >= 3.5
 * All OSes: [click here for installation instructions](https://cmake.org/install/)
* make >= 4.1
  * Linux: make is installed by default on most Linux distros
  * Mac: [install Xcode command line tools to get make](https://developer.apple.com/xcode/features/)
  * Windows: [Click here for installation instructions](http://gnuwin32.sourceforge.net/packages/make.htm)
* gcc/g++ >= 5.4
  * Linux: gcc / g++ is installed by default on most Linux distros
  * Mac: same deal as make - [install Xcode command line tools]((https://developer.apple.com/xcode/features/)
  * Windows: recommend using [MinGW](http://www.mingw.org/)
* [uWebSockets](https://github.com/uWebSockets/uWebSockets)
  * Run either `./install-mac.sh` or `./install-ubuntu.sh`.
  * If you install from source, checkout to commit `e94b6e1`, i.e.

```
git clone https://github.com/uWebSockets/uWebSockets 
cd uWebSockets
git checkout e94b6e1
```

Some function signatures have changed in v0.14.x. See [this PR](https://github.com/udacity/CarND-MPC-Project/pull/3) for more details.
* Simulator. You can download these from the [project intro page](https://github.com/udacity/self-driving-car-sim/releases) in the classroom.

## Basic Build Instructions

1. Clone this repo.
2. Make a build directory: `mkdir build && cd build`
3. Compile: `cmake .. && make`
4. Run it: `./pid`. 

## Reflections

### P, I, D coefficients
After playing around the P, I, D coefficients, I found that P is something like a force factor which shows how strong the controllor will pull back the object (like a car in this case); while I is something dealing with systematic error; and D makes the control convergent. 

### Tuning PID for steering and another PD for throttle
Firstly, I use the twiddle algorithm to find the PID, however, after a couple of frames, the car convergents quickly but also oscillates a lot. Then I manually finetune the parameters, trying to make the car moves smoothly rather than convergents sharply. I tried a lot and found a set of PID params as (0.1, 0.001, 1). With the throttle set to be constant as 0.3, the car drives okay, but it sometimes oscillate when turning at a big steering angle. 

To make the result  better, then I make a **PD controllor** to adjust throttle. My initial purpose was that the car should slow down when steering. The reason I took PD instead of PID to control throttle was that I just need an adjusting value adding to a default throttle, which was set to be 2, since a high throttle will make car accelerate faster. The PD controllor adjust throttle just by the steering angle. PD params was set to be (2, 20) with experiments. Now, the result seems better, but the car also oscillate when driving very fast.

I wasn't satisfied yet. Another idea came out that when the car was at a high speed, one driver might want to brake and steer with a small angle to control the car. Then I set `pid.Kp` to be inversely proportional to speed, and `pd.Kp `proportional to speed. Thus, when the car moves fast and is going to steer, it will slow down and then steer with a small angle. As a result, the car steer smoothly. See the final result shown in [THIS VIDEO.](https://youtu.be/labXfaXSZeM)
