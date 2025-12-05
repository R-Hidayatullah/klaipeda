#include <iostream>
#include <tinyxml2.h>
using namespace tinyxml2;

#include <ft2build.h>
#include FT_FREETYPE_H

void testFreetype()
{
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        printf("Freetype init FAILED!\n");
        return;
    }
    printf("Freetype init OK!\n");

    // Try loading any .ttf
    if (FT_New_Face(ft, "C:/Windows/Fonts/arial.ttf", 0, nullptr))
    {
        printf("Load font FAILED\n");
    }
    else
    {
        printf("Font load OK\n");
    }

    FT_Done_FreeType(ft);
}

void testTinyXML2()
{
    const char *xml =
        "<root>"
        "  <child name='abc'>123</child>"
        "</root>";

    XMLDocument doc;
    if (doc.Parse(xml) == XML_SUCCESS)
    {
        printf("TinyXML2 OK\n");
        auto *child = doc.FirstChildElement("root")->FirstChildElement("child");
        printf("child text = %s\n", child->GetText());
    }
    else
    {
        printf("TinyXML2 FAILED!\n");
    }
}

int main()
{
    std::cout << "Hello World!\n";
    testFreetype();
    testTinyXML2();
    return 0;
}