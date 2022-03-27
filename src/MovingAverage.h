// MovingAverage.h
#include <CircularBuffer.h>

/**
 * T should be either float or double, because of division
 */
template <typename T, size_t S, typename IT = typename CircularBuffer<T, S>::index_t> class MovingAverage {
public:
  static constexpr IT capacity = static_cast<IT>(S);
  using index_t = IT;

  // Disables copy constructor, assignment operator
  MovingAverage(const MovingAverage&) = delete;
  MovingAverage(MovingAverage&&) = delete;
  MovingAverage& operator=(const MovingAverage&) = delete;
  MovingAverage& operator=(MovingAverage&&) = delete;

  // fields
private:
  CircularBuffer<T, S> partial_averages;
  T current_average;

public:
  constexpr MovingAverage() : current_average(0) {
  }

  T getFastAverage() {
    return current_average;
  }

  T getAverage() {
    current_average = 0;
    for (index_t i = 0; i < partial_averages.size(); i++) {
      current_average += partial_averages[i];
    }
    return current_average;
  }

  void push(T value) {
    T next_partial_average = value / S;
    if (partial_averages.isEmpty()) {
      // first use, so fill the buffer
      for (index_t i = 0; i < S; i++) {
        partial_averages.push(next_partial_average);
      }
      current_average = value;
    } else {
      // buffer must be full
      current_average -= partial_averages.pop();
      partial_averages.unshift(next_partial_average);
      current_average += next_partial_average;
    }
  }

  bool is_filled_with(T value) {
    // Serial.printf("is_filled_with( %.20g )\n", value);

    // Serial.printf("  partial_averages.size() == %d\n", partial_averages.size());
    // Serial.printf("  S == %d\n", S);
    if (partial_averages.size() != S) {
      return false;
    }

    T partial_average_fill = value / S;
    // Serial.printf("  partial_average_fill == %d\n", partial_average_fill);

    for (index_t i = 0; i < partial_averages.size(); i++) {
      // Serial.printf("  partial_averages[%d] == %.20g\n", i, partial_averages[i]);
      if (partial_averages[i] != partial_average_fill) {
        return false;
      }
    }
    return true;
  }

};
