/************************************************************************/
/* pei::EngineLite (©)Copyright 2009-12 Jürgen Schober                  */
/*                                                                      */
/* This Software is distributed as Freeware. It may be copied and       */
/* distributed (even as a companion to a commercial product) as long as */
/* the archive stays complete. No further agreement is necessary.       */
/************************************************************************/

#ifndef PEI_IMAGEG_SEQUENCE_DECODER_H
#define PEI_IMAGEG_SEQUENCE_DECODER_H

#include <profile_config.h>

#include <decoder/dec_video.h>

#include <render/render_surface.h>

#include <string>
#include <vector>

#include <boost/weak_ptr.hpp>

namespace pei
{
        class ImageSequenceAsset;
        typedef boost::shared_ptr<ImageSequenceAsset> ImageSequenceAssetPtr;

        class ImageSequenceAsset : public VideoAsset
        {
        private:
            ImageSequenceAsset( const ImageSequenceAsset& other );

        protected:
            std::string                  m_DirectoryName;
            std::string                  m_ResourceName;
            int                          m_MaxMemUsed;
            bool 				     	 m_CacheFrames;
            Format                   	 m_Format; // global (default) format descriptor
            std::vector<std::string> 	 m_FileNames;
            std::vector<pei::SurfacePtr> m_FrameCache;
            bool                         m_IsOpen;
            std::string                  m_Extension;

            ImageSequenceAsset( const char* dir, bool load_to_memory = true );
            ImageSequenceAsset( const char* dir, const std::vector<std::string>& names, bool load_to_memory );
            ImageSequenceAsset( const char* pattern, int from, int to, bool load_to_memory );

        public:
            virtual ~ImageSequenceAsset();

            //////////////////////////////////////////////////////////////////////////
            // Asset Interface

            virtual bool CanHandle();

            virtual bool Open(  );

            virtual void Close( );

            virtual int GetMemoryUsage();

            /** abstract format description */
            virtual std::string GetFormatString() const = 0;

            virtual const char* GetResourceName() const {
                return m_ResourceName.c_str();
            }

            virtual const char* GetResourceDirectory() const {
                return m_DirectoryName.c_str();
            }
            //////////////////////////////////////////////////////////////////////////
            // VisualAsset interface

            /** get the number of frames in the video asset. Single frame returns 1 */
            virtual int GetNumberOfFrames() const;

            /** animation/movie time in ms */
            virtual double GetLenght() const;

            /** read current frames per second */
            virtual double GetFps() const;

            /** override fps for decoder */
            virtual void SetFps( double fps );

            /** get a format description for the given frame. 0 if single image */
            virtual const pei::Format& GetFormat( ) const;

            /** decode a frame to the given format */
            virtual pei::SurfacePtr Decode( pei::Format& dest_format, double time = 0 );

        protected:
            virtual pei::SurfacePtr ReadFrame( const char* file_name ) = 0;

            virtual pei::SurfacePtr DecodeFrame( int i );
        };
}; // namespace pei

#endif // PEI_IMAGEG_SEQUENCE_DECODER_H
