int luaopen_compat53_utf8(lua_State *L);
int luaopen_lpeg(lua_State *L);
int luaopen__cqueues(lua_State *L);
int luaopen__cqueues_errno(lua_State *L);
int luaopen__cqueues_auxlib(lua_State *L);
int luaopen__openssl_rand(lua_State *L);
int luaopen__openssl_ssl(lua_State *L);
int luaopen__openssl_ssl_context(lua_State *L);
int luaopen__openssl_pkey(lua_State *L);
int luaopen__openssl_x509_verify_param(lua_State *L);
int luaopen__openssl_x509_cert(lua_State *L);
int luaopen__openssl_x509_name(lua_State *L);
int luaopen__openssl_x509_altname(lua_State *L);
int luaopen__openssl_bignum(lua_State *L);
int luaopen_compat53_string(lua_State *L);
int luaopen_compat53_table(lua_State *L);

static const luaL_Reg LUAPAK_PRELOADED_LIBS[] = {
  { "compat53.utf8", luaopen_compat53_utf8 },
  { "_cqueues", luaopen__cqueues},
  { "_cqueues.errno", luaopen__cqueues_errno},
  { "_cqueues.auxlib", luaopen__cqueues_auxlib},
  { "_openssl.rand", luaopen__openssl_rand},
  { "_openssl.ssl", luaopen__openssl_ssl},
  { "_openssl.ssl.context", luaopen__openssl_ssl_context},
  { "_openssl.pkey", luaopen__openssl_pkey},
  { "_openssl.x509.verify_param", luaopen__openssl_x509_verify_param},
  { "_openssl.x509.cert", luaopen__openssl_x509_cert},
  { "_openssl.x509.name", luaopen__openssl_x509_name},
  { "_openssl.x509.altname", luaopen__openssl_x509_altname},
  { "_openssl.bignum", luaopen__openssl_bignum},
  { "lpeg", luaopen_lpeg },
  { "compat53.string", luaopen_compat53_string },
  { "compat53.table", luaopen_compat53_table },
  { NULL, NULL }
};
