# InteractiveMapEditor
A tool that can help you create and edit simple interactive maps and schemes
 
Created using Qt 5.15.0 and MinGW 8.1.0


Tool allows you to transform any picture with certain contrast between borders and background automatically into the map with interactive regions. After that you will be able to add infomation that is connected to these regions, change region colors, make regions non-interactive, save/load maps created using this tool. You can see examples in videos below


Colors that are too bright are transformed into background, when analyzing image, colors that are less bright are treated as borders of regions:

https://github.com/komitydev/InteractiveMapEditor/assets/69326421/b0b05fe6-0417-41e2-aba0-745083874884


Here is the example of map creation process:

https://github.com/komitydev/InteractiveMapEditor/assets/69326421/66d8dae5-67a3-4c72-97d2-7bd085001d37


Here you can see how to fill map with pre-collected information from csv file automatically - program takes information and assigns it to the region if the name of the region from csv file is found in the currently opened map:

https://github.com/komitydev/InteractiveMapEditor/assets/69326421/89d23d28-6ba9-44e6-a0e8-5205bc20b45a


Here is the example where you can see the load of completed map, that was created in this tool and saved before. It has some information in tables, that are connected to regions. Regions have custom colors and some of them are disabled so you cannot interact with them (lakes and other small regions). You can also navigate to search for different regions:

https://github.com/komitydev/InteractiveMapEditor/assets/69326421/619c19dc-75ef-4c40-a57d-0881dcb4c364
