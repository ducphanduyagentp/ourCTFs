# Notes - got_it

It is full relro so we can't overwrite the GOT. However, we can overwrite entry in the FILE jump table.
Follow the last puts call to overwrite it to onegadget. Notice the onegadget constraints when it is called.
