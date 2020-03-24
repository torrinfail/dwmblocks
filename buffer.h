#ifndef ___BUFFER_H___
#define ___BUFFER_H___

#include <string.h>

#define BUFFER(N)\
	struct \
	{ \
		char data[N]; \
		size_t count; \
	}

#define buffer_eq(X, Y) \
({ \
	typeof(X) __x = (X); \
	typeof(Y) __y = (Y); \
	__x->count != __y->count ? 0 : !memcmp(__x->data, __y->data, __y->count); \
})

#define buffer_copy(X, Y)\
({\
	typeof(X) __x = (X);\
	typeof(Y) __y = (Y);\
	memcpy(__x->data, __y->data, __y->count);\
	__x->count = __y->count;\
	__x;\
})

#define buffer_copy_str(X, S, L)\
({\
	typeof(X) __x = (X);\
	char const* __s = (S);\
	size_t __l = (L);\
	memcpy(__x->data, __s, __l);\
	__x->count = __l;\
	__x;\
})

#define buffer_append(X, Y)\
({\
	typeof(X) __x = (X);\
	typeof(Y) __y = (Y);\
	memcpy(__x->data + __x->count, __y->data, __y->count);\
	__x->count += __y->count;\
	__x;\
})

#define buffer_append_str(X, S, L)\
({\
	typeof(X) __x = (X);\
	char const* __s = (S);\
	size_t __l = (L);\
	memcpy(__x->data + __x->count, __s, __l);\
	__x->count += __l;\
	__x;\
})

#endif /* ___BUFFER_H___ */
