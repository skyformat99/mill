#define MAX_INPUT_LEN 4096


static unsigned char *nonce[crypto_box_NONCEBYTES]; /* nonce      */
static char nhex[crypto_box_NONCEBYTES * 2 + 1];    /* nonce hex  */
static unsigned char pk[crypto_box_PUBLICKEYBYTES]; /* public key */
static unsigned char sk[crypto_box_SECRETKEYBYTES]; /* secret key */
static char key[crypto_box_PUBLICKEYBYTES * 2 + 1];
static size_t psz = crypto_box_PUBLICKEYBYTES * 2 + 1;
static size_t ssz = crypto_box_SECRETKEYBYTES * 2 + 1;
static size_t ksz = 48;

/*
 * print_hex() is a wrapper around sodium_bin2hex() which allocates
 * temporary memory then immediately prints the result followed by \n
 */
static void print_hex(const void *bin, const size_t bin_len) {
  char   *hex;
  size_t  hex_size;

  if ( bin_len >= SIZE_MAX / 2 )
    abort();

  hex_size = bin_len * 2 + 1;
  if ((hex = (char *)malloc(hex_size)) == NULL)
    abort();

  /* the library supplies a few utility functions like the one below */
  if (sodium_bin2hex(hex, hex_size, (unsigned char *)bin, bin_len) == NULL)
    abort();

  puts(hex);
  free(hex);
}

/* nbuf uses a pointer kept back in the binding */
static char *nbuf() {
  size_t sz = crypto_box_NONCEBYTES;
  randombytes_buf(&nonce, sz);
  sodium_bin2hex(nhex, sz * 2 + 1, (unsigned char *)nonce, sz);
  return nhex;
}

size_t reinterpret_msg (const char *input, char *cast) {
  size_t len = strlen(input);
  memcpy(cast, input, len);
  return len;
}

//unsigned long * h2bin (unsigned char *bin, MaybeLocal<String>hex, unsigned long *len) {
//  sodium_hex2bin( (unsigned char *)&bin, 4096, (char *)hex,
//                  sizeof hex * 2 + 1, NULL, len, NULL);
//  return len;
//}
//sodium_hex2bin(ppk, MAX_INPUT_LEN, buf, nbytes, NULL, NULL, NULL);

/* return a nonce hex to node */
NAN_METHOD(nstr){
  char *n = nbuf();
  ret(New(n).ToLocalChecked());
}

/* return the crypto_box_primitive */
NAN_METHOD(box_primitive){
  const char *box_primitive = crypto_box_primitive();
  ret(New( box_primitive ).ToLocalChecked());
}

/* return the sodium_version */
NAN_METHOD(sodium_version){
  const char *sodium_version = sodium_version_string();
  ret(New( sodium_version ).ToLocalChecked());
}

NAN_METHOD(setkeys){
  utf8 kp(info[0]);
  utf8 ks(info[1]);

  sodium_hex2bin((unsigned char *)&pk, 4096, *kp, psz, NULL, NULL, NULL);
  sodium_hex2bin((unsigned char *)&sk, 4096, *ks, ssz, NULL, NULL, NULL);
}

NAN_METHOD(getkeys){
  Local<Object> o = New<Object>();

  sodium_bin2hex((char *)&key, ksz * 2 +1, (unsigned char *)pk, sizeof pk);
  Set(o, New("pk").ToLocalChecked(), New<String>(key).ToLocalChecked());

  sodium_bin2hex((char *)&key, ksz * 2 +1, (unsigned char *)sk, sizeof sk);
  Set(o, New("sk").ToLocalChecked(), New<String>(key).ToLocalChecked());
  ret(o);
}
