/************************************************************************/
/* pei::Engine (©)Copyright 2009-11 Jürgen Schober                      */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef ASSET_INTERFACE_H
#define ASSET_INTERFACE_H

/************************************************************************/
/* Raw asset interface class                                            */
/************************************************************************/

#include <string>

#include <boost/shared_ptr.hpp>

namespace pei
{
    class Asset
    {
    private:
        Asset(const Asset& );
    protected:
        // must create shared_ptr!!
        Asset() {}

        Asset( const char* name ) {} 

    public:
        virtual ~Asset() {}

        /** check if the assert can handle the data associated */
        virtual bool CanHandle() = 0;

        /** open the resource */
        virtual bool Open( ) = 0;

        /** close the resource handle */
        virtual void Close( ) = 0;

        /** memory need to load the asset */
        virtual int GetMemoryUsage() = 0;

        /** abstract format description */
        virtual std::string GetFormatString() const = 0;

        /** get the stored resource name */
        virtual const char* GetResourceName() const = 0;

        /** get the stored resource name */
        virtual const char* GetResourceDirectory() const = 0;
    };
    typedef boost::shared_ptr<Asset> AssetPtr;

}; //namespace pei

#endif // ASSET_INTERFACE_H
