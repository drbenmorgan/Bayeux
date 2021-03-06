=============================
Bayeux 3.4.5 Release Notes
=============================

Bayeux 3.4.5 adds some new features as well as fixes for reported issues.

For information on changes made in previous versions, please see
the `release notes archive`_.

.. _`release notes archive` : archived_notes/index.rst

.. contents:

Requirements
============

Bayeux  3.4.5 requires  Linux (recommended  Ubuntu >=18.04  (preferred
20.04), CentOS >=7.5), or macOS 10.


Additions
=========

* Add ``geomtools::plain_model`` class.
* Add                ``geomtools::i_model::destroy``               and
  ``geomtools::i_model::_at_destroy``  methods  for possible  cleaning
  from within a specific model.
* Add  load/store  methods in  ``geomtools::tessellated_solid``  class
  (using  a  simple  ASCII  format,  alternative  to  the  STL  import
  features).
* Add ``geomtools::model_with_internal_items_tools::init_internal_items`` method
  to enable automatic setup of internal physical volumes without using initialization
  through a ``datatools::properties`` object (like in ``geomtools::model_with_internal_items_tools::plug_internal_models``).


  
Removals
=========


Changes
=======

* Better support for model cleaning in ``geomtools::model_factory``.

Fixes
=====
    
Bugs
====


.. end
