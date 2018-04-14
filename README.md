# Dulkan
A collection of class wrappers and helper functions as an intermediate interface with the Vulkan computer graphics API

Dulkan began as (and largely still is) an exercise for me to learn the Vulkan computer graphics API. At the time of creation of this repository, the project is a windows-only, standalone program that draws an orange triangle (following one of the test image examples from Pawel Lapinski's Vulkan Cookbook). It is missing a lot of the obviously needed features of what a proper Vulkan interface would need, including but far from limited to descriptor sets, uniform buffers, and indexed drawing. These and many other items will be items for me to add as I work on producing an image less boring than a triangle.

The expected use case will eventually be to contain your application's code to the implementation of three functions in a derived class from a base DkApplication class. The developer will implement an init(), a finalize(), and a draw() function. There will presumably also need to be a set of callbacks available for override for keyboard and mouse interfacing, screen resizing, and more.

This is all purely theoretical until this project gets a little more fleshed out.
