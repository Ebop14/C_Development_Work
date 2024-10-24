****************
* EXTENSION USED
****************

I have three memory leaks that I know of
One is from when I extract an individual word from the sentence. I have no idea why it leaks, and if I delete it it deletes it from the bag.

The other two stem from my index_load and index_delete. although I call index_delete(loadedIndex), it still leaks memory. 
