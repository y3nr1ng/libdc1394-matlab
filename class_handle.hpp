#ifndef CLASS_HANDLE_HPP
#define CLASS_HANDLE_HPP

#include "mex.h"

#define CLASS_SIGNATURE 0xFF00F0A5
template <class base_class> class class_handle
{
public:
	class_handle(base_class *ptr)
		: ptr_m(ptr), name_m(typeid(base_class).name()) {
		signature_m = CLASS_HANDLE_SIGNATURE;
	}
	~class_handle() {
		signature_m = 0;
		delete ptr_m;
	}
	bool is_valid() {
		return ((signature_m == CLASS_SIGNATURE) &&
		        !strcmp(name_m.c_str(), typeid(base_class).name()));
	}
	base_class *ptr() {
		return ptr_m;
	}

private:
	uint32_t signature_m;
	std::string name_m;
	base_class *ptr_m;
};

template <class base_class> inline mxArray *ptr2mat(base_class *ptr) {
	mexLock();
	mxArray *out = mxCreateNumericMatrix(1, 1, mxUINT64_CLASS, mxREAL);
	*((uint64_t *)mxGetData(out)) = reinterpret_cast<uint64_t>(new class_handle<base_class>(ptr));
	return out;
}

template <class base_class> inline class_handle<base_class> *mat2handle_ptr(const mxArray *in) {
	if(mxGetNumberOfElements(in) != 1 ||
	   mxGetClassID(in) != mxUINT64_CLASS ||
	   mxIsComplex(in))
	{
		mexErrMsgTxt("Input must be a real uint64 scalar.");
	}
	class_handle<base_class> *ptr = reinterpret_cast<class_handle<base_class> *>(*((uint64_t *)mxGetData(in)));
	if(!ptr->is_valid())
		mexErrMsgTxt("Handle not valid.");
	return ptr;
}

template <class base_class> inline base_class *mat2ptr(const mxArray *in) {
	return mat2handle_ptr<base_class>(in)->ptr();
}

template <class base_class> inline void destroy_object(const mxArray *in) {
	delete mat2handle_ptr<base_class>(in);
	mexUnlock();
}

#endif
