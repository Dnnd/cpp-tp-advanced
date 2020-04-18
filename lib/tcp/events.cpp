#include "tcp/events.hpp"
EventsSet::EventsSet(std::initializer_list<Events> events) : events_{0} {
  for (Events event : events) {
    events_ |= static_cast<uint32_t>(event);
  }
}

void EventsSet::sub(Events event) noexcept {
  events_ &= ~static_cast<uint32_t>(event);
}
void EventsSet::add(Events event) noexcept {
  events_ |= static_cast<uint32_t>(event);
}
EventsSet EventsSet::diff(EventsSet other) const noexcept {
  uint32_t diff = other.events_ ^ events_;
  return EventsSet{diff};
}
uint32_t EventsSet::data() const noexcept { return events_; }
bool EventsSet::empty() const noexcept { return events_ == 0; }
bool EventsSet::haveIntersection(EventsSet event) const noexcept {
  return (events_ & event.events_) != 0;
}
EventsSet::EventsSet(uint32_t events) : events_{events} {}
bool EventsSet::contains(Events event) const noexcept {
  return events_ & static_cast<uint32_t>(event);
}
bool operator==(EventsSet lhs, EventsSet rhs) {
  return lhs.events_ == rhs.events_;
}
