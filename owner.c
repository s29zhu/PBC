#include <pbc.h>
#include <pbc_test.h>

typedef struct {
  field_t Fq, Fq2, Eq;
  int exp2, exp1;
  int sign1;
} *a_pairing_data_ptr;

typedef struct {
  field_ptr field; // The field where the curve is defined.
  element_t a, b;  // The curve is E: Y^2 = X^3 + a X + b.
  mpz_ptr cofac;
  element_t gen_no_cofac;
  element_t gen;
  mpz_ptr quotient_cmp;
} *curve_data_ptr;

int main(int argc, char **argv) {
  pairing_t pairing;
  double t0, t1;
  element_t g, OSK, alpha, OPK, s, a;//, beta_invert;
  FILE *fp;

  pbc_demo_pairing_init(pairing, argc, argv);
  if (!pairing_is_symmetric(pairing)) pbc_die("pairing must be symmetric");
  
  element_init_Zr(alpha, pairing);
  element_init_Zr(s, pairing);
  element_init_Zr(a, pairing);
  element_init_G1(g, pairing);
  element_init_G1(OSK, pairing);
  element_init_GT(OPK, pairing);
  element_set(g, ((curve_data_ptr)((a_pairing_data_ptr) pairing->data)->Eq->data)->gen);
 // element_printf("The generator of elliptic curve points group is %B\n", g);
 
 t0 = pbc_get_time(); 
 element_random(alpha);
 element_random(s);
// element_printf("random alpha is %B\n", alpha);
 element_pow_zn(OSK, g, alpha);
 element_pairing(OPK, g, g);
 element_pow_zn(OPK, OPK, alpha);
// element_printf("OSK is %B\nOPK is %B\n", OSK, OPK);
 t1 = pbc_get_time();
 printf("Time cost %fs\n", t1-t0);
 element_clear(g);
 element_clear(alpha);
 element_clear(OSK);
 element_clear(OPK);
}
