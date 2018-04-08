/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2017 - ROLI Ltd.

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/

namespace juce
{

//==============================================================================
/**
    A type of InputSource that represents a URL.

    @see InputSource
*/
class JUCE_API  URLInputSource     : public InputSource
{
public:
    //==============================================================================
    /** Creates a URLInputSource for a url. */
    URLInputSource (const URL& url);

    /** Move constructor which will move the URL into the InputSource.

        This is useful when the url carries any security credentials.
    */
    URLInputSource (URL&& url);

    /** Destructor. */
    ~URLInputSource();

    InputStream* createInputStream();
    InputStream* createInputStreamFor (const String& relatedItemPath);
    int64 hashCode() const;

private:
    //==============================================================================
    const URL u;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (URLInputSource)
};

}
