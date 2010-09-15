#ifndef STDAFX_H_
#define STDAFX_H_
#include <string>
using std::string;

#include <vector>
using std::vector;

#include <bitset>

#include <set>

#include <json/json.h>
#include <zoidcom.h>
#include <v8.h>
#include <irrlicht.h>
using namespace irr;

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

#include <cproxyv8.h>

#endif
