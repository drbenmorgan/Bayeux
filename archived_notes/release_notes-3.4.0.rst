=============================
Bayeux 3.4.0 Release Notes
=============================

Bayeux 3.4 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux 3.4.0 requires Linux (for example Ubuntu 18.04 or later),
or macOS 10.


Additions
=========

* The ``datatools::factory`` class now has an enriched registration mechanism
  with the ability to register the ``std::type_info`` object associated to
  a registrered class factory. A templatized registration method is provided
  to automate the instantiation of a factory registration record.

  - Two new methods, both named ``datatools::factory::fetch_type_id``, are provided
    to fetch a class factory registration ID by its type info or by the proper
    template class parameter.
  - Factory macros have been adapted to this new interface.
  - Several client classes using the factory registration system
    have been updated too.

* The ``datatools::handle`` class is enriched by a standard smart pointer
  interfaces, including a ``datatools::make_handle`` template method (PR #21).

* The ``mctools::simulated_data`` class uses now the new smart ``print_tree`` interface.

* The ``mctools::simulated_data`` class can now host collections of handles of hits of any class
  inherited from the ``mctools::base_step_hit`` class. This should enable to store
  specialized hit objects. Template methods have been added. Test programs have been updated.
  
Removals
=========

* Some unused methods have been removed from the ``datatools::properties`` class.
  

Changes
=======

* The ``mctools::base_step_hit`` class is upgraded to serialization version 2
  with more supported attributes in place of using auxiliary properties. This should
  make this class more efficient while using it from the Geant4 engine.
  In consequence, several algorithms have been adapted to use the new set of attributes.


Fixes
=====

None.

