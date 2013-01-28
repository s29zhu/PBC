#include "message_handle.h"
//#include "stdlib.h"
void messageToValue(void *message, mpz_t message_mpz, char *m){
  char* c = NULL;
  unsigned int value = 0, size = 0;
  c = message;
//  gmp_printf("The initial decimal value of the message is: %Zd.\n", message_mpz);
  while(*c != '\0'){
  	value = (unsigned int) *c;
	mpz_mul_ui(message_mpz, message_mpz, 256);
  	mpz_add_ui(message_mpz, message_mpz, value);
	c += 1;
  }
 // size = 8 * message_mpz->_mp_size;
  //m = (char* )malloc(size);
  mpz_get_str(m, 10, message_mpz);
//  printf("The hex str is %s\n", m);
//   gmp_printf("The final result value of the message is: %Zd.\n", message_mpz);
}

void valueToMessage(char *message, mpz_t message_mpz){
  char *c = NULL;
  c = (char*) message_mpz->_mp_d;
  unsigned int count = 0;

  while(*(c + count) != '\0'){
	count += 1;
  }
  message[count] = '\0';
  while(count > 0){
  	count -= 1;
  	message[count] = *c;
	c += 1;
  }
//  printf("%s\n", message);
}

/*int main(int argc, char **argv){
  mpz_t message_mpz;
  void* raw_message = "hello, world,fajohuiuy8w.\0";
  char message_hex[2048];
  mpz_init(message_mpz);
  messageToValue(raw_message, message_mpz, message_hex);
  valueToMessage(message_hex, message_mpz);

  printf("%s\n", (char *)message_hex);
  mpz_clear(message_mpz);
  return 1;
}*/
