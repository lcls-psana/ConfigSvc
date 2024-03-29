#ifndef CONFIGSVC_CONFIGSVC_H
#define CONFIGSVC_CONFIGSVC_H

//------------------------------
// Description:
//	Class ConfigSvc.
//------------------------------

//-----------------
// C/C++ Headers --
//-----------------
#include <list>
#include <memory>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>

//----------------------
// Base Class Headers --
//----------------------


//-------------------------------
// Collaborating Class Headers --
//-------------------------------
#include "ConfigSvc/ConfigSvcImplI.h"
#include "ConfigSvc/ConfigSvcTypeTraits.h"
#include "ConfigSvc/Exceptions.h"

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------

//		---------------------
// 		-- Class Interface --
//		---------------------

namespace ConfigSvc {

/// @addtogroup ConfigSvc

/**
 *  @ingroup ConfigSvc
 *
 *  Configuration service class.
 *
 *  This software was developed for the LCLS project.  If you use all or
 *  part of it, please give an appropriate acknowledgment.
 *
 *  @version $Id$
 *
 *  @author Andrei Salnikov
 */

class ConfigSvc  {
public:

  typedef int context_t;

  // helper classes to handle conversion
  class Result {
  public:
    Result(const boost::shared_ptr<const std::string>& pstr, const std::string& sect,
        const std::string& parm) : m_pstr(pstr), m_sect(sect), m_parm(parm) {}

    // conversion to final value
    template <typename T>
    operator T() const { return ConfigSvcTypeTraits<T>::fromString(*m_pstr, m_sect, m_parm); }

  private:
    boost::shared_ptr<const std::string> m_pstr;
    const std::string& m_sect;
    const std::string& m_parm;
  };

  template <typename Def>
  class ResultDef {
  public:
    ResultDef(const boost::shared_ptr<const std::string>& pstr, const std::string& sect,
        const std::string& parm, const Def& def)
      : m_pstr(pstr), m_sect(sect), m_parm(parm), m_def(def) {}

    // conversion to final value
    template <typename T>
    operator T() const {
      if (m_pstr) {
        return ConfigSvcTypeTraits<T>::fromString(*m_pstr, m_sect, m_parm);
      } else {
        return m_def;
      }
    }

    // retursn true if result is a default value
    bool isDefault() const { return not m_pstr; }

  private:
    boost::shared_ptr<const std::string> m_pstr;
    const std::string& m_sect;
    const std::string& m_parm;
    Def m_def;
  };

  class ResultList {
  public:
    ResultList(const boost::shared_ptr<const std::list<std::string> >& pstr, const std::string& sect,
        const std::string& parm)
    : m_pstr(pstr), m_sect(sect), m_parm(parm) {}

    // conversion to final value
    // can't templatize conversion operator because of ambiguities with
    // std::initializer_list starting in c++11
    operator std::list<std::string>() const {
      return to_container<std::list<std::string>>();
    }
    operator std::vector<std::string>() const {
      return to_container<std::vector<std::string>>();
    }
    operator std::list<unsigned long int>() const {
      return to_container<std::list<unsigned long int>>();
    }
    operator std::vector<unsigned long int>() const {
      return to_container<std::vector<unsigned long int>>();
    }
    operator std::list<long int>() const {
      return to_container<std::list<long int>>();
    }
    operator std::vector<long int>() const {
      return to_container<std::vector<long int>>();
    }
    operator std::list<float>() const {
      return to_container<std::list<float>>();
    }
    operator std::vector<float>() const {
      return to_container<std::vector<float>>();
    }
    operator std::list<double>() const {
      return to_container<std::list<double>>();
    }
    operator std::vector<double>() const {
      return to_container<std::vector<double>>();
    }
    operator std::list<unsigned>() const {
      return to_container<std::list<unsigned>>();
    }
    operator std::vector<unsigned>() const {
      return to_container<std::vector<unsigned>>();
    }
    operator std::list<int>() const {
      return to_container<std::list<int>>();
    }
    operator std::vector<int>() const {
      return to_container<std::vector<int>>();
    }
    operator std::list<bool>() const {
      return to_container<std::list<bool>>();
    }
    operator std::vector<bool>() const {
      return to_container<std::vector<bool>>();
    }

  private:
    template <typename Container>
    Container to_container() const {
      Container res ;
      for (std::list<std::string>::const_iterator it = m_pstr->begin() ; it != m_pstr->end() ; ++ it ) {
        res.push_back( ConfigSvcTypeTraits<typename Container::value_type>::fromString(*it, m_sect, m_parm) );
      }
      return res;
    }

    boost::shared_ptr<const std::list<std::string> > m_pstr;
    const std::string& m_sect;
    const std::string& m_parm;
  };

  template <typename Def>
  class ResultListDef {
  public:
    ResultListDef(const boost::shared_ptr<const std::list<std::string> >& pstr, const std::string& sect,
        const std::string& parm, const Def& def)
      : m_pstr(pstr), m_sect(sect), m_parm(parm), m_def(def) {}

    // conversion to final value
    // disable std::initializer_list since it creates template ambiguities
    // starting with c++11
    template <typename Container,
      typename std::enable_if<!std::is_same<Container, std::initializer_list<std::string>>::value, int>::type = 0>
    operator Container() const {
      if (not m_pstr.get()) return m_def;

      Container res ;
      for (std::list<std::string>::const_iterator it = m_pstr->begin() ; it != m_pstr->end() ; ++ it ) {
        res.push_back( ConfigSvcTypeTraits<typename Container::value_type>::fromString(*it, m_sect, m_parm) );
      }
      return res;
    }

  private:
    boost::shared_ptr<const std::list<std::string> > m_pstr;
    const std::string& m_sect;
    const std::string& m_parm;
    Def m_def;
  };


  /**
   *  @brief Make instance of configuration service for a specified or global context.
   *
   *  Multiple instances of configuration service with the same context are identical.
   *  If context is not provided then service from global context is instantiated.
   *  If the service has not been initialized yet in a given context the exception is
   *  generated.
   *
   *  @param[in] context Optional context, if not specified global context is used.
   *
   *  @throw ExceptionNotInitialized
   */
  ConfigSvc(context_t context = context_t(0)) : m_ctx(context) {}

  /**
   *  @brief Get the value of a single parameter.
   *
   *  The method returns instance of Result class which is convertible
   *  to any standard numeric or boolean types.
   *  This method will throw ExceptionMissing if parameter is not there
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @return Result class convertible to numeric types.
   *
   *  @throw ExceptionMissing
   */
  Result get(const std::string& section, const std::string& param) const {
    boost::shared_ptr<const std::string> pstr = impl(m_ctx).get(section, param);
    if (not pstr.get()) throw ExceptionMissing(section, param);
    return Result( pstr, section, param );
  }

  /**
   *  @brief Get the value of a single parameter as a string.
   *
   *  This method will throw ExceptionMissing if parameter is not there
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @return Instance of std::string class
   *
   *  @throw ExceptionMissing
   */
  std::string getStr(const std::string& section, const std::string& param) const {
    boost::shared_ptr<const std::string> pstr = impl(m_ctx).get(section, param);
    if (not pstr.get()) throw ExceptionMissing(section, param);
    return *pstr;
  }

  /**
   *  @brief Get the value of a single parameter, use default if not there.
   *
   *  The method returns instance of ResultDef class which is convertible
   *  to any standard numeric or boolean types. If parameter (or whole section)
   *  is missing in configuration the default value is returned.
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @param[in] def      Default value returned if parameter is not found
   *  @return ResultDef class convertible to numeric types.
   */
  template <typename T>
  ResultDef<T> get(const std::string& section, const std::string& param, const T& def) const
  {
    boost::shared_ptr<const std::string> pstr = impl(m_ctx).get(section, param);
    return ResultDef<T>( pstr, section, param, def );
  }

  /**
   *  @brief Get the string value of a single parameter, use default if not there.
   *
   *  If parameter (or whole section) is missing in configuration the default value is returned.
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @param[in] def      Default value returned if parameter is not found
   *  @return Instance of std::string class
   */
  std::string getStr(const std::string& section, const std::string& param, const std::string& def) const
  {
    boost::shared_ptr<const std::string> pstr = impl(m_ctx).get(section, param);
    if ( pstr.get() ) {
      return *pstr;
    } else {
      return def;
    }
  }

  /**
   *  @brief Get the value of a single parameter as sequence.
   *
   *  The method returns instance of ResultList class which is convertible
   *  to any standard container (vector, list) of numeric or boolean types.
   *  This method will throw ExceptionMissing if parameter is not there
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @return ResultList class convertible to sequences of numeric types.
   *
   *  @throw ExceptionMissing
   */
  ResultList getList(const std::string& section, const std::string& param) const {
    boost::shared_ptr<const std::list<std::string> > pstr = impl(m_ctx).getList(section, param);
    if (not pstr.get()) throw ExceptionMissing(section, param);
    return ResultList(pstr, section, param);
  }

  /**
   *  @brief Get the value of a single parameter as sequence, or return default value.
   *
   *  The method returns instance of ResultListDef class which is convertible
   *  to any standard container (vector, list) of numeric or boolean types.
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @param[in] def      Default value returned if parameter is not found
   *  @return ResultList class convertible to sequences of numeric types.
   */
  template <typename Cont>
  ResultListDef<Cont> getList(const std::string& section, const std::string& param, const Cont& def) const
  {
    boost::shared_ptr<const std::list<std::string> > pstr = impl(m_ctx).getList(section, param);
    return ResultListDef<Cont>(pstr, section, param, def);
  }

  /**
   *  @brief Set the value of the parameter.
   *
   *  New parameter (and section) will be created if necessary, if parameter already exists
   *  its value will be replaced. Note that this method only changes in-memory presentation
   *  of the configuration parameters. If parameters come from come external storage (file)
   *  the values of parameters in that storage do not change.
   *
   *  @param[in] section  Configuration section name
   *  @param[in] param    Configuration parameter name
   *  @param[in] value    New parameter value
   */
  void put(const std::string& section, const std::string& param, const std::string& value)
  {
    impl(m_ctx).put(section, param, value);
  }

  /**
   *  @brief Get a list of all parameter name in a section.
   *
   *  Returns an empty list if the section is not found.
   *
   *  @param[in] section  Configuration section name
   *  @return List of strings.
   */
  std::list<std::string> getKeys(const std::string& section) const
  {
    return impl(m_ctx).getKeys(section);
  }

protected:

private:

  context_t m_ctx;

//------------------
// Static Members --
//------------------

public:

  /**
   *  @brief Initialize the service in global or specified context.
   *
   *  This method has to be called exactly once for any context.  If service has been
   *  initialized already in a given context the exception is thrown.
   *
   *  NOTE: this method is not thread safe, run all initialization for one thread please.
   *
   *  @param[in] impl    Pointer to instance of implementation class.
   *  @param[in] context Optional context, if not specified global context is used.
   *  @throw ExceptionInitialized
   */
  static void init(const boost::shared_ptr<ConfigSvcImplI>& impl, context_t context = context_t(0));

  /**
   *  @brief Check if service is initialized in global or specified context.
   *
   *  NOTE: this method is not thread safe, run all initialization for one thread please.
   *
   *  @param[in] context Optional context, if not specified global context is used.
   *  @return True if service was initialized already.
   */
  static bool initialized(context_t context = context_t(0));

private:

  // get the implementation instance, throws if not initialized
  static ConfigSvcImplI& impl(context_t context);

};

} // namespace ConfigSvc

#endif // CONFIGSVC_CONFIGSVC_H
