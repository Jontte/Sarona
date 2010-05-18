#include <string>

#include <json/json.h>

#include <v8.h>

#include <irrlicht.h>
using namespace irr;

#include <btBulletDynamicsCommon.h>


#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;

#include <boost/intrusive_ptr.hpp>
using boost::intrusive_ptr;
using boost::get_pointer;

#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;

#include <boost/bind.hpp>
#include <boost/function.hpp>

void intrusive_ptr_add_ref(IReferenceCounted* t);
void intrusive_ptr_release(IReferenceCounted* t);

