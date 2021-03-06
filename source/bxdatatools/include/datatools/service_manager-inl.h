/// \file datatools/service_manager-inl.h
/* Author(s)     : Francois Mauger <mauger@lpccaen.in2p3.fr>
 * Creation date : 2011-06-07
 * Last modified : 2015-12-03
 */

#ifndef DATATOOLS_SERVICE_MANAGER_INL_H
#define DATATOOLS_SERVICE_MANAGER_INL_H

namespace datatools {

  //----------------------------------------------------------------------
  // class service_manager template method implementations.

  template <class T>
  bool service_manager::is_a(const std::string& name_) const
  {
    const base_service & serv = get_service(name_);
    const std::type_info& ti = typeid(T);
    const std::type_info& tf = typeid(serv);
    return (ti == tf);
  }

  template<class T>
  T& service_manager::grab(const std::string& name_)
  {
    return dynamic_cast<T&>(grab_service(name_));
  }

  template<class T>
  const T& service_manager::get(const std::string& name_) const
  {
    service_manager* sm = const_cast<service_manager*>(this);
    return const_cast<T&>(sm->grab<T>(name_));
  }

  template <class ServiceClass>
  void service_manager::register_service_type(const std::string& id_)
  {
    _factory_register_.registration(id_, boost::factory<ServiceClass*>());
    return;
  }

}  // end of namespace datatools

#endif // DATATOOLS_SERVICE_MANAGER_INL_H

// Local Variables: --
// mode: c++ --
// c-file-style: "gnu" --
// tab-width: 2 --
// End: --
