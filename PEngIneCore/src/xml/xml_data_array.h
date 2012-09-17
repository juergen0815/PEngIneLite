/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/
#ifndef XML_DATA_ARRAY_H
#define XML_DATA_ARRAY_H

#include <render/render_node.h>

#include <xml/xml_aux.h>

#include <vector>
#include <string>

#include <stdlib.h>

namespace pei {
	class XmlDataArray;
	typedef boost::shared_ptr<XmlDataArray> XmlDataArrayPtr;

    //////////////////////////////////////////////////////////////////////////
    // abstract container
    class DataContainerInterface
    {
    public:
        virtual ~DataContainerInterface() {}

        virtual void add( const std::string& s ) = 0;

        virtual unsigned int size() = 0;

        virtual void assign( const char* c, int num ) = 0;

        template < class T >
        const std::vector<T>& getData() const;
    };
    typedef boost::shared_ptr<DataContainerInterface> DataContainerInterfacePtr;

    //////////////////////////////////////////////////////////////////////////
    // template container

    template< class T >
    class DataContainer : public DataContainerInterface
    {
    protected:
        std::vector< T > m_Data;
    public:
        virtual ~DataContainer() {}

        virtual void add( const std::string& s )
        {
            if ( s.length() > 0 ) {
                m_Data.push_back( valueOf(s) );
            }
        }

        virtual const std::vector<T>& getData() const = 0;

        virtual unsigned int size() { return m_Data.size(); };

        virtual void assign( const char* c, int num )
        {
            m_Data.assign( (const T*)c, (const T*)(c+num) );
        }

    protected:
        virtual T valueOf( const std::string& v ) const = 0;
    };

    //////////////////////////////////////////////////////////////////////////
    // typed containers

    class ByteDataContainer : public DataContainer<char> {
    protected:
        virtual char valueOf( const std::string& v ) const { return (char)atoi(v.c_str()); }
    public:
        virtual ~ByteDataContainer() {}

        virtual const std::vector<char>& getData() const   { return m_Data;}

        virtual std::vector<char>& getData() { return m_Data;}
    };
    typedef boost::shared_ptr<ByteDataContainer> ByteDataContainerPtr;

    class ShortDataContainer : public DataContainer<short> {
    protected:
        virtual short valueOf( const std::string& v ) const { return (short)atoi(v.c_str()); }
    public:
        virtual ~ShortDataContainer() {}

        virtual const std::vector<short>& getData() const   { return m_Data;}

        virtual std::vector<short>& getData() { return m_Data;}
    };
    typedef boost::shared_ptr<ShortDataContainer> ShortDataContainerPtr;

    class IntDataContainer : public DataContainer<int> {
    protected:
        virtual int valueOf( const std::string& v ) const { return (int)atoi(v.c_str()); }
    public:
        virtual ~IntDataContainer() {}

        virtual const std::vector<int>& getData() const   { return m_Data;}

        virtual std::vector<int>& getData() { return m_Data;}
    };
    typedef boost::shared_ptr<IntDataContainer> IntDataContainerPtr;

    class FloatDataContainer : public DataContainer<float> {
    protected:
        virtual float valueOf( const std::string& v ) const { return (float)atof(v.c_str()); }
    public:
        virtual ~FloatDataContainer() {}

        virtual const std::vector<float>& getData() const { return m_Data;}

        virtual std::vector<float>& getData() { return m_Data;}
    };
    typedef boost::shared_ptr<FloatDataContainer> FloatDataContainerPtr;

    class DoubleDataContainer : public DataContainer<double> {
    protected:
        virtual double valueOf( const std::string& v ) const { return (double)atof(v.c_str()); }
    public:
        virtual ~DoubleDataContainer() {}

        virtual const std::vector<double>& getData() const   { return m_Data;}

        virtual std::vector<double>& getData() { return m_Data;}
    };
    typedef boost::shared_ptr<DoubleDataContainer> DoubleDataContainerPtr;

 	class XmlDataArray : public pei::XmlAux
 	{
    public:
        enum {
            TP_BYTE = 0x1000,   // = GL_UNSIGNED_BYTE,
            TP_SHORT,  // GL_UNSIGNED_SHORT,
            TP_INT,     // GL_UNSIGNED_INT,
            TP_FLOAT,  // GL_FLOAT,
            TP_DOUBLE, // GL_DOUBLE,
        };
    protected:
        //////////////////////////////////////////////////////////////////////////
        // storage
		DataContainerInterfacePtr m_DataContainer;
		int                       m_Stride;
        int                       m_Type;
        std::string               m_Separator;
	public:
		XmlDataArray( const char* separator = ",", int type = TP_FLOAT );

		virtual ~XmlDataArray() {}

		virtual int GetType() const	{ return m_Type; }

		virtual int GetStride() const { return m_Stride; }

		virtual unsigned int GetSize() const { return  ( m_DataContainer != NULL ) ? m_DataContainer->size() : 0; }

		virtual DataContainerInterfacePtr GetDataContainer();

        //////////////////////////////////////////////////////////////////////////
        // quick cast
        template<class T> boost::shared_ptr<T> as() const { return boost::dynamic_pointer_cast<T>(m_DataContainer); }
#if 0
		virtual const char* asByteArray() const;

		virtual const short* asShortArray() const;

		virtual const int* asIntArray() const;

		virtual const float* asFloatArray() const;
#endif

//		virtual const double* asDoubleArray() const;

		virtual bool xmlSetAttribute( const char* attribute, const char* value );

        virtual void xmlHandleTextSection( const char* text );

		virtual bool xmlInit( const char* base, XmlNode *node = NULL );

	};

//     template< class S = std::string("'"), class T = XmlDataArray::TP_FLOAT >
//     class XmlDataArrayT : public XmlDataArray
//     {
//     public:
//         XmlDataArrayT : XmlDataArray( S.c_str(), T ) {}
//     };
}

#endif /* XML_DATA_ARRAY_H */
