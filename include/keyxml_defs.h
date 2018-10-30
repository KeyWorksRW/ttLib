// Definitions used by CKeyXml,

#define PARSE_MINIMAL			0x00000000 // Unset the following flags.
#define PARSE_PI				0x00000002 // Parse '<?...?>'
#define PARSE_DOCTYPE			0x0000000  // Parse '<!DOCTYPE ...>' section, setting '[...]' as data member.
#define PARSE_COMMENTS			0x00000008 // Parse <!--...-->'
#define PARSE_CDATA				0x00000010 // Parse '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_ESCAPES			0x00000020 // Not implemented.
#define PARSE_TRIM_PCDATA		0x00000040 // Trim '>...<'
#define PARSE_TRIM_ATTRIBUTE	0x00000080 // Trim 'foo="..."'.
#define PARSE_TRIM_CDATA		0x00000100 // Trim '<![CDATA[...]]>', and/or '<![INCLUDE[...]]>'
#define PARSE_TRIM_ENTITY		0x00000200 // Trim '<!ENTITY name ...>', etc.
#define PARSE_TRIM_DOCTYPE		0x00000400 // Trim '<!DOCTYPE [...]>'
#define PARSE_TRIM_COMMENT		0x00000800 // Trim <!--...-->'
#define PARSE_NORMALIZE			0x00001000 // Normalize all entities that are flagged to be trimmed.
#define PARSE_DTD				0x00002000 // If PARSE_DOCTYPE set, then parse whatever is in data member ('[...]').
#define PARSE_DTD_ONLY			0x00004000 // If PARSE_DOCTYPE|PARSE_DTD set, then parse only '<!DOCTYPE [*]>'
#define PARSE_DEFAULT			0x0000FFFF
#define PARSE_DONT_SET			0x80000000

// #pragma warning(disable: 4480)	// nonstandard extension used: specifying underlying type for enum

enum HTML_ELEMENT : size_t
{
	ELEMENT_UNKNOWN,	// < Unknown tag!
	ELEMENT_A,			// < A
	ELEMENT_ABBR,		// < ABBR
	ELEMENT_ACRONYM,	// < ACRONYM
	ELEMENT_ADDRESS,	// < ADDRESS
	ELEMENT_ALIGN,		// < ALIGN
	ELEMENT_APPLET,		// < APPLET
	ELEMENT_AREA,		// < AREA
	ELEMENT_B,			// < B
	ELEMENT_BASE,		// < BASE
	ELEMENT_BASEFONT,	// < BASEFONT
	ELEMENT_BDO,		// < BDO
	ELEMENT_BGSOUND,	// < BGSOUND
	ELEMENT_BIG,		// < BIG
	ELEMENT_BLINK,		// < BLINK
	ELEMENT_BLOCKQUOTE,	// < BLOCKQUOTE
	ELEMENT_BODY,		// < BODY
	ELEMENT_BR,			// < BR
	ELEMENT_BUTTON,		// < BUTTON
	ELEMENT_CAPTION,	// < CAPTION
	ELEMENT_CENTER,		// < CENTER
	ELEMENT_CITE,		// < CITE
	ELEMENT_CODE,		// < CODE
	ELEMENT_COL,		// < COL
	ELEMENT_COLGROUP,	// < COLGROUP
	ELEMENT_COMMENT,	// < COMMENT
	ELEMENT_DD,			// < DD
	ELEMENT_DEL,		// < DEL
	ELEMENT_DFN,		// < DFN
	ELEMENT_DIR,		// < DIR
	ELEMENT_DIV,		// < DIF
	ELEMENT_DL,			// < DL
	ELEMENT_DT,			// < DT
	ELEMENT_EM,			// < EM
	ELEMENT_EMBED,		// < EMBED
	ELEMENT_FIELDSET,	// < FIELDSET
	ELEMENT_FONT,		// < FONT
	ELEMENT_FORM,		// < FORM
	ELEMENT_FRAME,		// < FRAME
	ELEMENT_FRAMESET,	// < FRAMESET
	ELEMENT_H1,			// < H1
	ELEMENT_H2,			// < H2
	ELEMENT_H3,			// < H3
	ELEMENT_H4,			// < H4
	ELEMENT_H5,			// < H5
	ELEMENT_H6,			// < H6
	ELEMENT_HEAD,		// < HEAD
	ELEMENT_HR,			// < HR
	ELEMENT_HTML,		// < HTML
	ELEMENT_I,			// < I
	ELEMENT_IFRAME,		// < IFRAME
	ELEMENT_ILAYER,		// < ILAYER
	ELEMENT_IMG,		// < IMG
	ELEMENT_INPUT,		// < INPUT
	ELEMENT_INS,		// < INS
	ELEMENT_ISINDEX,	// < ISINDEX
	ELEMENT_KBD,		// < KBD
	ELEMENT_KEYGEN,		// < KEYGEN
	ELEMENT_LABEL,		// < LABEL
	ELEMENT_LAYER,		// < LAYER
	ELEMENT_LEGEND,		// < LEGEND
	ELEMENT_LI,			// < LI
	ELEMENT_LINK,		// < LINK
	ELEMENT_LISTING,	// < LISTING
	ELEMENT_MAP,		// < MAP
	ELEMENT_MARQUEE,	// < MARQUEE
	ELEMENT_MENU,		// < MENU
	ELEMENT_META,		// < META
	ELEMENT_MULTICOL,	// < MULTICOL
	ELEMENT_NEXTID,		// < NEXTID
	ELEMENT_NOBR,		// < NOBR
	ELEMENT_NOEMBED,	// < NOEMBED
	ELEMENT_NOFRAMES,	// < NOFRAMES
	ELEMENT_NOLAYER,	// < NOLAYER
	ELEMENT_NOSAVE,		// < NOSAVE
	ELEMENT_NOSCRIPT,	// < NOSCRIPT
	ELEMENT_OBJECT,		// < OBJECT
	ELEMENT_OL,			// < OL
	ELEMENT_OPTGROUP,	// < OPTGROUP
	ELEMENT_OPTION,		// < OPTION
	ELEMENT_P,			// < P
	ELEMENT_PARAM,		// < PARAM
	ELEMENT_PLAINTEXT,	// < PLAINTEXT
	ELEMENT_PRE,		// < PRE
	ELEMENT_Q,			// < Q
	ELEMENT_RB,			// < RB
	ELEMENT_RBC,		// < RBC
	ELEMENT_RP,			// < RP
	ELEMENT_RT,			// < RT
	ELEMENT_RTC,		// < RTC
	ELEMENT_RUBY,		// < RUBY
	ELEMENT_S,			// < S
	ELEMENT_SAMP,		// < SAMP
	ELEMENT_SCRIPT,		// < SCRIPT
	ELEMENT_SELECT,		// < SELECT
	ELEMENT_SERVER,		// < SERVER
	ELEMENT_SERVLET,	// < SERVLET
	ELEMENT_SMALL,		// < SMALL
	ELEMENT_SPACER,		// < SPACER
	ELEMENT_SPAN,		// < SPAN
	ELEMENT_STRIKE,		// < STRIKE
	ELEMENT_STRONG,		// < STRONG
	ELEMENT_STYLE,		// < STYLE
	ELEMENT_SUB,		// < SUB
	ELEMENT_SUP,		// < SUP
	ELEMENT_TABLE,		// < TABLE
	ELEMENT_TBODY,		// < TBODY
	ELEMENT_TD,			// < TD
	ELEMENT_TEXTAREA,	// < TEXTAREA
	ELEMENT_TFOOT,		// < TFOOT
	ELEMENT_TH,			// < TH
	ELEMENT_THEAD,		// < THEAD
	ELEMENT_TITLE,		// < TITLE
	ELEMENT_TR,			// < TR
	ELEMENT_TT,			// < TT
	ELEMENT_U,			// < U
	ELEMENT_UL,			// < UL
	ELEMENT_VAR,		// < VAR
	ELEMENT_WBR,		// < WBR
	ELEMENT_XMP,		// < XMP
	ELEMENT_NOLOC,	    // < NOLOC
	ELEMENT_XML,	    // < XML

	// MSHelp: tags

	ELEMENT_MSH_LINK,	    // < MSHelp:link
	ELEMENT_MSH_TAG,		// < All single MSHelp: tags

	ELEMENT_UNKNOWN_XML_TAG,	// < any other unknown tag (presumably XML)

	// Following are for Sitemap files

	ELEMENT_FOLDER,
	ELEMENT_PAGE,

	//	Note: we could add MAML elements next if it made parsing easier
};

enum : size_t {
	DOCTYPE_XHTML_STRICT,
	DOCTYPE_XHTML_TRANSITIONAL,
	DOCTYPE_HTML_STRICT,
	DOCTYPE_HTML_TRANSITIONAL,
};

enum XMLENTITY : size_t
{
	ENTITY_NULL,						// An undifferentiated entity.
	ENTITY_ROOT,						// A document tree's absolute root.
	ENTITY_ELEMENT,						// E.g. '<...>'
	ENTITY_PCDATA,						// E.g. '>...<'
	ENTITY_CDATA,						// E.g. '<![CDATA[...]]>'
	ENTITY_COMMENT,						// E.g. '<!--...-->'
	ENTITY_PI,							// E.g. '<?...?>'
	ENTITY_INCLUDE,						// E.g. '<![INCLUDE[...]]>'
	ENTITY_DOCTYPE,						// E.g. '<!DOCTYPE ...>'.
	ENTITY_DTD_ENTITY,					// E.g. '<!ENTITY ...>'.
	ENTITY_DTD_ATTLIST,					// E.g. '<!ATTLIST ...>'.
	ENTITY_DTD_ELEMENT,					// E.g. '<!ELEMENT ...>'.
	ENTITY_DTD_NOTATION					// E.g. '<!NOTATION ...>'.
};
