#ifndef randomNavigation_HPP
#define randomNavigation_HPP

#include <zmq.hpp>

//Defines


//Prototypes
void dataLoop(int fd, zmq::socket_t& pub);

#endif
