#ifndef ENCODER_HANDLER_H
#define ENCODER_HANDLER_H

// Initialize encoder pins and state
void initEncoder();

// Main encoder processing function - call from loop()
void handleEncoder();

#endif
