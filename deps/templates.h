#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

using namespace v8;

Handle<Value> Socket(const Arguments& args) {
  HandleScope scope;

  int type = args[1]->Int32Value();

  int ret = nn_socket(args[0]->Int32Value(), type);
  if(type == NN_SUB)
    assert(nn_setsockopt (ret, NN_SUB, NN_SUB_SUBSCRIBE, "", 0) == 0);

  return scope.Close( Local<Value>::New(Integer::New(ret)) );
}

Handle<Value> Close(const Arguments& args) {
  HandleScope scope;

//  if(args.Length() != 1) {
//    ThrowException(Exception::TypeError(String::New("Must pass one variable")));
//    return scope.Close(Undefined());
//  }
//  if(!args[0]->IsNumber()) {
//    ThrowException(Exception::TypeError(String::New("Must pass [integer]")));
//    return scope.Close(Undefined());
//  }

  int sp = args[1]->Int32Value();

  int ret = nn_close(sp);

  return scope.Close( Local<Value>::New(Integer::New(ret)) );
}

Handle<Value> SetSockOpt(const Arguments& args) {
  HandleScope scope;

  int sock = args[0]->Int32Value();
  int level = args[1]->Int32Value();
  int option = args[2]->Int32Value();
  int optval = args[3]->Int32Value();

  int rc = nn_setsockopt (sock, level, option, (const void *)&optval, sizeof(optval));

  return scope.Close( Local<Value>::New(Integer::New(rc)) );
}

Handle<Value> GetSockOpt(const Arguments& args) {
  HandleScope scope;

  int sock = args[0]->Int32Value();
  int level = args[1]->Int32Value();
  int option = args[2]->Int32Value();

  int optval;
  size_t optvallen;
  nn_getsockopt(sock, level, option, (void *)&optval, &optvallen);

  return scope.Close( Local<Value>::New(Integer::New(optval)) );
}


Handle<Value> Bind(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value address(args[1]);

  int ret = nn_bind(args[0]->Int32Value(), *address);

  return scope.Close( Local<Value>::New(Integer::New(ret)) );
}

Handle<Value> Connect(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value address(args[1]);

  int ret = nn_connect(args[0]->Int32Value(), *address);

  return scope.Close( Local<Value>::New(Integer::New(ret)) );
}

Handle<Value> Send(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value message(args[1]);
  unsigned char *buf = (unsigned char *)nn_allocmsg (message.length(), 0);
  if(buf == NULL) // Out Of Memory
    return scope.Close( Local<Value>::New(Integer::New(-1)) );

  memcpy(buf, *message, message.length());

  int rc = nn_send (args[0]->Int32Value(), &buf, NN_MSG, 0);

  return scope.Close( Local<Value>::New(Integer::New(rc)) );
}

Handle<Value> Recv(const Arguments& args) {
  HandleScope scope;

  String::Utf8Value message(args[1]);
  unsigned char *buf;

  int len = nn_recv (args[0]->Int32Value(), &buf, NN_MSG, 0);

  return scope.Close( Local<Value>::New(String::New((const char *)buf, len)) );
}

Handle<Value> Errno(const Arguments& args) {
  HandleScope scope;

  if(args.Length() > 0)
  {
    ThrowException(Exception::TypeError(String::New("This function does not takes arguments")));
    return scope.Close(Undefined());
  }

  int err = nn_errno();

  return scope.Close( Local<Value>::New(Integer::New(err)) );
}

Handle<Value> StrErr(const Arguments& args) {
  HandleScope scope;

  if(args.Length() != 1)
  {
    ThrowException(Exception::TypeError(String::New("Must pass one variable")));
    return scope.Close(Undefined());
  }

  if(!args[0]->IsNumber())
  {
    ThrowException(Exception::TypeError(String::New("Must pass [integer]")));
    return scope.Close(Undefined());
  }

  int errnum = args[0]->Int32Value();

  const char *errstr = nn_strerror(errnum);

  return scope.Close( Local<Value>::New(String::New(errstr)) );
}