#ifndef __EUI64_H__
#define __EUI64_H__

extern EmberEui64 emLocalEui64;

// this reads the token and caches it in RAM
void emInitEui64(void);

// modify the RAM cache but don't write the token
void emSetEui64(EmberEui64 *eui64);

#endif  // __EUI64_H__

