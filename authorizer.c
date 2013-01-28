#include <pbc.h>
#include <pbc_test.h>
#include <stdio.h>
#include <math.h>
#include <gmp.h>
#include <string.h>
#include "message_handle.h"

typedef struct {
  field_t Fq, Fq2, Eq;
  int exp2, exp1;
  int sign1;
} *a_pairing_data_ptr;

typedef struct {
  field_ptr field; // The field where the curve is defined.
  element_t a, b;  // The curve is E: Y^2 = X^3 + a X + b.
  // cofac == NULL means we're using the whole group of points.
  // otherwise we're working in the subgroup of order #E / cofac,
  // where #E is the number of points in E.
  mpz_ptr cofac;
  // A generator of E.
  element_t gen_no_cofac;
  // A generator of the subgroup.
  element_t gen;
  // A non-NULL quotient_cmp means we are working with the quotient group of
  // order #E / quotient_cmp, and the points are actually coset
  // representatives. Thus for a comparison, we must multiply by quotient_cmp
  // before comparing.
  mpz_ptr quotient_cmp;
} *curve_data_ptr;

int main(int argc, char **argv) {
  pairing_t pairing;
  double t0, t1;
  element_t g, h, f, beta, beta_invert, r, OSK, alpha, OPK, s, a, A, cd_pair,
		C, C_bar, D, C_temp, message_ele;
  char raw_message[2048] = "Secret message.\0";
  char message_dec[2048], message[2048];
  mpz_t message_mpz;// used when convert the decrypt value to string message

  t0 = pbc_get_time(); 
  pbc_demo_pairing_init(pairing, argc, argv);
  if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");

  mpz_init(message_mpz);
  element_init_Zr(beta, pairing);
  element_init_Zr(beta_invert, pairing);
  element_init_Zr(r, pairing);
  element_init_Zr(alpha, pairing);
  element_init_Zr(s, pairing);
  element_init_Zr(a, pairing);
  element_init_G1(g, pairing);
  element_init_G1(h, pairing);
  element_init_G1(f, pairing);
  element_init_G1(OSK, pairing);
  element_init_G1(D, pairing);
  element_init_G1(C, pairing);
  element_init_GT(OPK, pairing);
  element_init_GT(C_bar, pairing);
  element_init_GT(A, pairing);
  element_init_GT(cd_pair, pairing);
  element_init_GT(C_temp, pairing);
  element_init_GT(message_ele, pairing);

  // g is the generator of G1
  element_set(g, ((curve_data_ptr)((a_pairing_data_ptr) pairing->data)->Eq->data)->gen);
//private keys choose
  element_random(beta);//randomly picked up by authorizer
  element_random(alpha);//randomly picked up by owner
  element_random(s);//randomly chosen by owner
  element_random(a);//randomly chosen by owner
  element_random(r);//randomly chosen by authorizer
//authorizer public keys(G1, g, h = g^beta, f = g^(1/beta))
  element_invert(beta_invert, beta);
  element_pow_zn(h, g, beta);
  element_pow_zn(f, g, beta_invert);
//owner secret key g^alpha
  element_pow_zn(OSK, g, alpha);
//owner public key e(g, g)^alpha
  element_pairing(OPK, g, g);
  element_pow_zn(OPK, OPK, alpha);
// CT part compute
  element_pow_zn(C, h, s);//C = h^s=g^(beta*s)
 // c_bar compute
  printf("Before encryption. The raw message is:\n");
  puts(raw_message);
//map the raw message to an element of GT in the format of [message_dec,0]
  messageToValue(raw_message, message_mpz, message_dec);//get the hex str of raw message
  strcpy(message, "[");
  strcat(message, message_dec);
  strcat(message, ",0]");
  element_set_str(message_ele, message, 10);
// Do the encryption
  element_pow_zn(C_temp, OPK, s);//C_temp = e(g,g)^(alpha*s) 
  element_mul(C_bar, C_temp, message_ele);// C_bar = m*e(g,g)^(alpha*s)


//SK co-computed by authorizer and owner
  // compute D
  element_pow_zn(D, g, r);// computed by authorizer g^r
  element_pow_zn(D, D, a);//computed by owner g^(r*a)
  element_mul(D, D, OSK);//computed by owner g^(r*a + alpha)
  element_pow_zn(D, D, beta_invert);//computed by authorizer D = g^[(r*a + alpha)/beta]
//  element_printf("The secret key part D is %B\n", D);

//decrypt 
  // compute A
  element_pairing(A, g, g);
  element_pow_zn(A, A, r);
  element_pow_zn(A, A, a);
  element_pow_zn(A, A, s);// A = e(g, g)^(r*a*s)

  // 
  element_pairing(cd_pair, C, D);
  element_div(cd_pair, cd_pair, A);
  element_div(cd_pair, C_bar, cd_pair);//The decrypt message is cd_pair
//change the message_dec to string
  element_to_mpz(message_mpz, cd_pair);
  valueToMessage(message, message_mpz);
  printf("Decrypt successfully. The message is:\n");
  puts(message);
  t1 = pbc_get_time();
  printf("Time cost %fs\n", t1-t0);

  element_clear(g);
  element_clear(h);
  element_clear(f);
  element_clear(beta);
  element_clear(beta_invert);
  element_clear(r);
  element_clear(OSK);
  element_clear(alpha);
  element_clear(OPK);
  element_clear(s);
  element_clear(a);
  element_clear(A);
  element_clear(cd_pair);
  element_clear(C);
  element_clear(C_bar);
  element_clear(D);
  element_clear(C_temp);
  element_clear(message_ele);
  mpz_clear(message_mpz);
}
