/*
 * http://www.wpftutorial.net/MergedDictionaryPerformance.html
 * 생성일: 3/6/2023 1:16:19 PM
 *
 */

using System;
using System.Collections.Generic;
using System.Windows;

namespace SGToolsCommon
{
    /// <summary>
    /// The shared resource dictionary is a specialized resource dictionary
    /// that loads it content only once. If a second instance with the same source
    /// is created, it only merges the resources from the cache.
    /// </summary>
    public class SharedResourceDictionary : ResourceDictionary
    {
        /// <summary>
        /// Internal cache of loaded dictionaries
        /// </summary>
        public static Dictionary<Uri, ResourceDictionary> sharedDictionaries_ = new();

        /// <summary>
        /// Local member of the source uri
        /// </summary>
        private Uri sourceUri_ = null!;

        /// <summary>
        /// Gets or sets the uniform resource identifier (URI) to load resources from.
        /// </summary>
        public new Uri Source
        {
            get { return sourceUri_; }
            set
            {
                sourceUri_ = value;

                if (!sharedDictionaries_.ContainsKey(value))
                {
                    // If the dictionary is not yet loaded, load it by setting
                    // the source of the base class
                    base.Source = value;

                    // add it to the cache
                    sharedDictionaries_.Add(value, this);
                }
                else
                {
                    // If the dictionary is already loaded, get it from the cache
                    MergedDictionaries.Add(sharedDictionaries_[value]);
                }
            }
        }
    }
}
