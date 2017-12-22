SourceView

"SourceView" is a Cocoa sample application that demonstrates how to use NSOutlineView driven by NSTreeController and various other Cocoa classes to produce a Finder-like source view.

Among the features demonstrated are -

- Source outline view using NSOutlineView using a custom outline cell ImageAndTextCell,
- Loading dictionary data from disk using [NSDictionary dictionaryWithContentsOfFile…] to populate the outline view,
- Outline view uses selection highlight style: NSTableViewSelectionHighlightStyleSourceList, which gives is a gradient-style selection.
- Icon view of directories using NSCollectionView,
- Viewing URLs using WebView,
- Outline view drag and drop support for URLs and file system-based objects (from Safari, Finder, etc),
- Factoring or organizing its various NSViews into a separate nib using NSViewController,
- Factoring or organizing its various Windows into a separate nib using NSWindowController,
- Obtaining file system icons using NSWorkspace,
- NSplitView sub-pane size management during divider resize,
- Using template images in our buttons such as NSImageNameAddTemplate and NSImageNameRemoveTemplate,
- NSWindow edge alteration using [NSWindow setContentBorderThickness]


Sample Requirements
The supplied Xcode project was created using Xcode v3.2 or later running under Mac OS X 10.6.x or later.


Using the Sample
Simply build and run the sample using Xcode, launch SourceView and examine the contents in the outline view.
The first section "Devices" is automatically added with the help of: [[NSWorkspace sharedWorkspace] mountedLocalVolumePaths]. The second section "Places" is also automatically added by the window controller.

The sections following are determined by the external Outline.dict file, read in as a NSDictionary and populated into the NSOutlineView.  You can experiment on your own with adding and removing items in this file.


Changes from Previous Versions
1.0	First Release
1.1	Upgraded to support 10.6, now builds 3-way Universal (ppc, i386, x86_64), fixed a bug when removing a folder failed to update the detail view.


Feedback and Bug Reports
Please send all feedback about this sample by connecting to the Contact ADC page.
Please submit any bug reports about this sample to the Bug Reporting page.


Developer Technical Support
The Apple Developer Connection Developer Technical Support (DTS) team is made up of highly qualified engineers with development expertise in key Apple technologies. Whether you need direct one-on-one support troubleshooting issues, hands-on assistance to accelerate a project, or helpful guidance to the right documentation and sample code, Apple engineers are ready to help you.  Refer to the Apple Developer Technical Support page.
Copyright © 2010 Apple Inc. All rights reserved.