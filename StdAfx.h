#ifndef STDAFX_H_
#define STDAFX_H_

#include <string>
using std::string;

#include <vector>
using std::vector;

#include <bitset>
#include <set>
#include <memory>
using std::auto_ptr;

// JsonCpp json parsing
#include <json/json.h>

// Zoidcom networking
#include <zoidcom.h>

// V8 Javascript engine by Google
#include <v8.h>

// v8-juice, a v8 helper library
#include <v8/juice/convert.h>
#include <v8/juice/ClassWrap.h>
#include <v8/juice/forwarding.h>
#include <v8/juice/ClassWrap_JuiceBind.h>

// Irrlicht graphics engine
#include <irrlicht.h>
using namespace irr;

// Bullet physics engine
#include <btBulletDynamicsCommon.h>

#include <boost/shared_ptr.hpp>
using boost::shared_ptr;

#include <boost/make_shared.hpp>
using boost::make_shared;

#include <boost/scoped_ptr.hpp>
using boost::scoped_ptr;

#include <boost/intrusive_ptr.hpp>
using boost::intrusive_ptr;
using boost::get_pointer;

#include <boost/ptr_container/ptr_vector.hpp>
using boost::ptr_vector;

#include <boost/ptr_container/ptr_map.hpp>
using boost::ptr_map;

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>
#include <boost/preprocessor/seq/for_each.hpp>
#include <boost/preprocessor/tuple/elem.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/type_traits.hpp>

#ifndef ASSERT
#define ASSERT(x) assert(x)
#endif

#endif
