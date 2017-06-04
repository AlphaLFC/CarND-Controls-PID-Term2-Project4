#include <uWS/uWS.h>
#include <iostream>
#include "json.hpp"
#include "PID.h"
#include <math.h>

// for convenience
using json = nlohmann::json;

// For converting back and forth between radians and degrees.
constexpr double pi() { return M_PI; }
double deg2rad(double x) { return x * pi() / 180; }
double rad2deg(double x) { return x * 180 / pi(); }

// Checks if the SocketIO event has JSON data.
// If there is data the JSON object in string format will be returned,
// else the empty string "" will be returned.
std::string hasData(std::string s) {
  auto found_null = s.find("null");
  auto b1 = s.find_first_of("[");
  auto b2 = s.find_last_of("]");
  if (found_null != std::string::npos) {
    return "";
  }
  else if (b1 != std::string::npos && b2 != std::string::npos) {
    return s.substr(b1, b2 - b1 + 1);
  }
  return "";
}

int main()
{
  uWS::Hub h;

  PID pid, pd;
  // TODO: Initialize the pid variable.
  pid.Init(0.1, 0.001, 1);
  pd.Init(2, 0.0, 20);

  h.onMessage([&pid, &pd](uWS::WebSocket<uWS::SERVER> ws, char *data, size_t length, uWS::OpCode opCode) {
    // "42" at the start of the message means there's a websocket message event.
    // The 4 signifies a websocket message
    // The 2 signifies a websocket event
    if (length && length > 2 && data[0] == '4' && data[1] == '2')
    {
      auto s = hasData(std::string(data).substr(0, length));
      if (s != "") {
        auto j = json::parse(s);
        std::string event = j[0].get<std::string>();
        if (event == "telemetry") {
          // j[1] is the data JSON object
          double cte = std::stod(j[1]["cte"].get<std::string>());
          double speed = std::stod(j[1]["speed"].get<std::string>());
          double angle = std::stod(j[1]["steering_angle"].get<std::string>());
          double steer_value;
          double throttle = 2;
          /*
          * TODO: Calcuate steering value here, remember the steering value is
          * [-1, 1].
          * NOTE: Feel free to play around with the throttle and speed. Maybe use
          * another PID controller to control the speed!
          */
          // pid update and calculate steering
          pid.UpdateError(cte);
          steer_value = pid.TotalError();
          // tuning Kp, Ki, Kd for pid steering
          // Kp is inversely proportional to speed
          // at high speed, Kp will be low to avoid big oscillation
          // Ki set to be a factor of Kp
          // Kd remains to be constant
          // when Kp is too high, the car track convergent quikly but oscillate
          // when Kp is too low, the car may overshot the edge and pull over
          pid.Kp = 8 / (speed + 40);
          pid.Ki = pid.Kp / 100;

          // pd update and calculate throttle
          // when steering, adjust throttle
          pd.UpdateError(fabs(steer_value));
          throttle += pd.TotalError();
          // tuning Kp and Kd for pd throttle
          // when speed is high, pd will draw back throttle with strong force
          // keeping Kp low will make the car accelerate to a higher speed, but oscillating
          // keeping Kp high will slow down the car, but much more stable and no oscillating
          pd.Kp = speed / 8;
          pd.Kd = pd.Kp * 10;
          
          if (steer_value < -1)
            steer_value = -1;

          if (steer_value > 1)
            steer_value = 1;

          if (throttle > 2)
            throttle = 2;

          // DEBUG
          std::cout << "CTE:   " << cte << " Steering Value: " << steer_value << std::endl;
          std::cout << "Steer: " << steer_value << " Throttle Value: " << throttle << std::endl;
          std::cout << "Ks:    " << pid.Kp << " " << pid.Ki << " " << pid.Kd << std::endl;
          json msgJson;
          msgJson["steering_angle"] = steer_value;
          msgJson["throttle"] = throttle;
          auto msg = "42[\"steer\"," + msgJson.dump() + "]";
          std::cout << msg << std::endl;
          ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
        }
      } else {
        // Manual driving
        std::string msg = "42[\"manual\",{}]";
        ws.send(msg.data(), msg.length(), uWS::OpCode::TEXT);
      }
    }
  });

  // We don't need this since we're not using HTTP but if it's removed the program
  // doesn't compile :-(
  h.onHttpRequest([](uWS::HttpResponse *res, uWS::HttpRequest req, char *data, size_t, size_t) {
    const std::string s = "<h1>Hello world!</h1>";
    if (req.getUrl().valueLength == 1)
    {
      res->end(s.data(), s.length());
    }
    else
    {
      // i guess this should be done more gracefully?
      res->end(nullptr, 0);
    }
  });

  h.onConnection([&h](uWS::WebSocket<uWS::SERVER> ws, uWS::HttpRequest req) {
    std::cout << "Connected!!!" << std::endl;
  });

  h.onDisconnection([&h](uWS::WebSocket<uWS::SERVER> ws, int code, char *message, size_t length) {
    ws.close();
    std::cout << "Disconnected" << std::endl;
  });

  int port = 4567;
  if (h.listen(port))
  {
    std::cout << "Listening to port " << port << std::endl;
  }
  else
  {
    std::cerr << "Failed to listen to port" << std::endl;
    return -1;
  }
  h.run();
}
