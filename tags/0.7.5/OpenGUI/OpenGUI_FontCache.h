#pragma once
#ifndef B2AE3F25_D739_4f77_93B3_0930B8015415
#define B2AE3F25_D739_4f77_93B3_0930B8015415

#include "OpenGUI_PreRequisites.h"
#include "OpenGUI_Exports.h"
#include "OpenGUI_Types.h"
#include "OpenGUI_Imagery.h"
#include "OpenGUI_FontGlyph.h"

/*///////////////////////////////////////////////
A few quick notes on this design:
#1: The glyphmaps are not stored in an efficient data structure.
#2: The current design works.
#3: If you feel up to the task, please do alter the storage and lookups of the glyphmaps.
		It was implemented in this brute force method just to get it working as quickly
		as possible. A patch that sped this whole process up would be most welcomed.
///////////////////////////////////////////////*/

namespace OpenGUI{
	class Font;
	class FontAtlas;

	

	class FontCacheGlyphSet{
	public:
		Font* font;
		IVector2 glyphSize;
		typedef std::map<char,FontGlyph> GlyphMap;
		GlyphMap mGlyphMap;
	};


	//! \internal This class provides Allocation and Management of the Textures, Imagesets, and Imagery that are used to hold glyphs rendered by Freetype.
	/*! Basically, you can't draw a glyph to the screen unless it is first drawn onto a texture. This
		class performs texture space management, and allows Fonts to simply ask for storage space for
		glyph renderings, and it shall be provided.

	*/
	class FontCache
	{
	public:
		FontCache();
		~FontCache();
		void GetGlyph(Font* font, char glyph_charCode,
			const IVector2& glyph_pixelSize, FontGlyph& outFontGlyph);

		//! Flushes all glyphs from a given font
		void FlushFont(Font* font);
		
	private:
		IVector2 _calcNewAtlasSize(const IVector2& estimatedGlyphSize);
		FontCacheGlyphSet* _GetFontCacheGlyphSet(Font* font, const IVector2& glyph_pixelSize);
		void _RenderGlyph(FontCacheGlyphSet* glyphSet, char glyph_charCode);

		void _DestroyAllGlyphSets();
		void _DestroyAllFontAtlas();
		float _GetCurrentCacheEfficiency(); //returns a percentage of all font atlas coverage

		typedef std::list<FontCacheGlyphSet*> FontCacheGlyphSetList;
		FontCacheGlyphSetList mFontCacheGlyphSetList;

		typedef std::list<FontAtlas*> FontAtlasList;
		FontAtlasList mFontAtlasList;
	};

};//namespace OpenGUI{

#endif