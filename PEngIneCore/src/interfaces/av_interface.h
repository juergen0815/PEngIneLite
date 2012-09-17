/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef AV_INTERFACE_H
#define AV_INTERFACE_H

namespace pei
{
	class AvInterface
	{
	public:
	    /** @brief
	     *  @param
	     *  @return
	     */
		virtual ~AvInterface() {}

        /** @brief
         *  @param
         *  @return
         */
	    virtual void Start( bool resume = true ) = 0;

        /** @brief
         *  @param
         *  @return
         */
	    virtual void Stop( bool rewind = false ) = 0;

        /** @brief
         *  @param
         *  @return
         */
	    virtual void Rewind( ) = 0;

        /** @brief
         *  @param
         *  @return
         */
	    virtual void SetStreamPosition( double time, bool pause = true ) = 0;

        /** @brief
         *  @param
         *  @return
         */
        virtual double GetStreamPosition( ) const = 0;

        /** @brief
         *  @param
         *  @return
         */
	    virtual void SetPlayDirection( bool forward = true ) = 0;

        /** @brief
         *  @param
         *  @return
         */
        virtual double GetPlayDirection( ) const = 0;

        /** @brief
         *  @param
         *  @return
         */
        virtual bool IsRunning() const = 0;

        /** @brief
         *  @param
         *  @return
         */
        virtual void SetNumberOfLoops( unsigned int numLoops ) = 0;

        /** @brief
         *  @param
         *  @return
         */
        virtual unsigned int GetNumberOfLoops( ) const = 0;

	};
}; // namespace pei

#endif // AV_INTERFACE_H
