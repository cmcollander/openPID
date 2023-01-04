#include <cmath>
#include <algorithm>

class PID {
 public:
  // Constructor with default values for gains and integral limit
  PID(double kp = 0.0, double ki = 0.0, double kd = 0.0, double integral_limit = 0.0);

  // Update the PID controller with a new measurement and return the control output
  double Update(double measurement, double target);

  // Reset the integral term
  void Reset();

  // Set the integral limit
  void set_integral_limit(double integral_limit);

  double get_k_p();
  double get_k_i();
  double get_k_d();
  double get_integral_limit();

  void increment_k_p(double amount);
  void increment_k_i(double amount);
  void increment_k_d(double amount);
  void increment_integral_limit(double amount);

 private:
  // PID gains
  double kp_, ki_, kd_;
  // Integral limit
  double integral_limit_;
  // Previous error and integral term for derivative and integral calculations
  double prev_error_, integral_;
};
