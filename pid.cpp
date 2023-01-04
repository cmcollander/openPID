#include "pid.h"

PID::PID(double kp, double ki, double kd, double integral_limit)
    : kp_(kp), ki_(ki), kd_(kd), integral_limit_(integral_limit),
      prev_error_(0.0), integral_(0.0) {}

double PID::Update(double measurement, double target) {
    // Calculate the error
    double error = target - measurement;

    // Update the integral term
    integral_ += error;
    if (integral_limit_ > 0.0) {
        integral_ = std::clamp(integral_, -integral_limit_, integral_limit_);
    }

    // Calculate the derivative term
    double derivative = error - prev_error_;
    prev_error_ = error;

    // Calculate and return the control output
    return kp_ * error + ki_ * integral_ + kd_ * derivative;
}

void PID::Reset()
{
    integral_ = 0.0;
}

void PID::set_integral_limit(double integral_limit)
{
    integral_limit_ = integral_limit;
}

double PID::get_k_p()
{
    return kp_;
}

double PID::get_k_i()
{
    return ki_;
}

double PID::get_k_d()
{
    return kd_;
}

double PID::get_integral_limit()
{
    return integral_limit_;
}

void PID::increment_k_p(double amount)
{
    kp_ += amount;
    kp_ = kp_ < 0.0 ? 0.0 : kp_;
}

void PID::increment_k_i(double amount)
{
    ki_ += amount;
    ki_ = ki_ < 0.0 ? 0.0 : ki_;
}

void PID::increment_k_d(double amount)
{
    kd_ += amount;
    kd_ = kd_ < 0.0 ? 0.0 : kd_;
}

void PID::increment_integral_limit(double amount)
{
    integral_limit_ += amount;
    integral_limit_ = integral_limit_ < 0.0 ? 0.0 : integral_limit_;
}