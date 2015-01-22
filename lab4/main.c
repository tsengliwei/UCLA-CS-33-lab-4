//
//  main.c
//  lab4
//
//  Created by Li-Wei Tseng on 12/4/14.
//  Copyright (c) 2014 Li-Wei Tseng. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>

#define HEAPSIZE 32768

#define hdr_payload(i)  ((struct HDR *) &heap[i])->payload
#define hdr_freeall(i)  ((struct HDR *) &heap[i])->freeall
#define hdr_succesr(i)  ((struct HDR *) &heap[i])->succesr
#define hdr_previus(i)  ((struct HDR *) &heap[i])->previus

char *heap ;
int  heapsize ;

int sizehdr ;

struct HDR       // free block header/footer/linked list
{
    int  payload ;    // size of block (excluding headers)
    char freeall ;    // is this block allocated? 0=free/1=allocated
    int  succesr ;    // successor free block
    int  previus ;    // previous free block
} anchor ;

void dump_heap( char *t )   // prints all of the headers/footers on the heap
{
    int i ;
    
    int j=0 ;
    printf( "%s anchor= %d\n", t, anchor.succesr ) ;
    for( i=0; i<heapsize; i=i+sizehdr  )
        if ( hdr_payload(i) != 0 )
        {
            printf( "%6d %6d %6d %6d %6d\n", i, hdr_payload(i), hdr_freeall(i), hdr_previus(i), hdr_succesr(i) ) ;
            if ( j++%2==1 ) printf( "\n" ) ;
        }
}

void init_heap()   // initialize the heap
{
    heap = (char *) calloc( HEAPSIZE, 1 ) ;
    sizehdr =  16 ;
    heapsize = HEAPSIZE ;
    
    anchor.payload =  0 ;   // anchor
    anchor.freeall =  1 ;
    anchor.succesr =  0 ;
    anchor.previus = -1 ;
    
    hdr_payload(0) = heapsize-2*sizehdr ;   // header
    hdr_freeall(0) = 0 ;
    hdr_succesr(0) = heapsize ;
    hdr_previus(0) = -1 ;
    hdr_payload(heapsize-sizehdr) = heapsize-2*sizehdr ; // footer
    hdr_freeall(heapsize-sizehdr) = 0 ;
}



void lfree( int *aa )   // frees the block at address aa
{
    // lfree code here
    
    int addressAA = *aa; // point to payload
    int leftFooter = addressAA - 2 * sizehdr;
    int leftHeader = leftFooter - hdr_payload(leftFooter) - sizehdr;
    int aaHeader = addressAA - sizehdr;
   // printf("aaHeader: %d\n", aaHeader);
    //printf("hdr_payload(aaHeader): %d\n", hdr_payload(aaHeader));
    int aaFooter = addressAA + hdr_payload(aaHeader);
    //printf("addressAA + hdr_payload(aaHeader): %d\n", addressAA + hdr_payload(aaHeader));
    int rightHeader = addressAA + hdr_payload(aaHeader) + sizehdr;
    int rightFooter = rightHeader + sizehdr + hdr_payload(rightHeader);
    
    printf("leftHeader: %d\n", leftHeader);
    printf("leftFooter: %d\n", leftFooter);
    printf("aaHeader: %d\n", aaHeader);
    printf("aaFooter: %d\n", aaFooter);
    printf("rightHeader: %d\n", rightHeader);
    printf("rightFooter: %d\n", rightFooter);
    
    
    hdr_freeall(aaHeader) = 0;
    hdr_freeall(aaFooter) = 0;
    hdr_succesr(leftFooter) = 0;
    hdr_previus(leftFooter) = 0;
    hdr_succesr(aaFooter) = 0;
    hdr_previus(aaFooter) = 0;
    hdr_succesr(rightFooter) = 0;
    hdr_previus(rightFooter) = 0;
    
    if (leftHeader < 0) {
        hdr_freeall(leftHeader) = 1;
    }
    if (rightHeader >= heapsize) {
        hdr_freeall(rightHeader) = 1;
    }
    
   // int lastSuccesr = hdr_succesr(aaHeader);
    
    
//    // case 1: _AF -> F
//    if (leftHeader < 0 && hdr_freeall(rightHeader) == 0) {
//        hdr_freeall(leftHeader) = 1;
//    }
//    // case 2: FA_  -> F
//    
//    else if (hdr_freeall(rightHeader) == 0) {
//    
//    }
    
    
    
    // case 1: FA -> F
    if(aaHeader >= 16 && hdr_freeall(leftHeader) == 0 && rightHeader >= heapsize)
    {
        int sizea = hdr_payload(leftHeader);
        int sizea1 = hdr_payload(aaHeader);
        if(hdr_succesr(leftHeader) != heapsize)
        {
        hdr_previus(hdr_succesr(leftHeader)) = hdr_previus(leftHeader);
        }
        if(hdr_previus(leftHeader) != -1)
        {
        hdr_succesr(hdr_previus(leftHeader)) = hdr_succesr(leftHeader);
        }
        if(hdr_previus(leftHeader) != -1)
        {
        hdr_previus(leftHeader) = -1;
        hdr_succesr(leftHeader) = anchor.succesr;
        hdr_previus(anchor.succesr) = leftHeader;
        anchor.succesr = leftHeader;
            hdr_freeall(aaHeader) = 0;
            hdr_payload(aaHeader) = 0;
            hdr_previus(aaHeader) = 0;
            hdr_succesr(aaHeader) = 0;
            hdr_freeall(leftFooter) = 0;
            hdr_payload(leftFooter) = 0;
            hdr_previus(leftFooter) = 0;
            hdr_succesr(leftFooter) = 0;
            hdr_payload(leftHeader) = sizea + sizea1 + 32;
            hdr_freeall(aaFooter) = 0;
            hdr_payload(aaFooter) = hdr_payload(leftHeader);
            hdr_previus(aaFooter) = 0;
            hdr_succesr(aaFooter) = 0;
        }
    }
    // case 3: AAA -> AFA
     if (hdr_freeall(leftHeader) == 1 && hdr_freeall(rightHeader) == 1) {
        
        hdr_succesr(aaHeader) = anchor.succesr; // point to anchor's successor
        hdr_previus(aaHeader) = -1; // become the first item of the list
        
        hdr_previus(anchor.succesr) = aaHeader;
        anchor.succesr = aaHeader;
        
    }
    // case 4: AAF -> AF
    else if (hdr_freeall(leftHeader) == 1) {
        hdr_payload(aaHeader) += 2 * sizehdr + hdr_payload(rightHeader);
        hdr_payload(aaFooter) = 0;
        hdr_payload(rightHeader) = 0;
        hdr_payload(rightFooter) = hdr_payload(aaHeader);
        
        // if the block to be freed is the last one, update the last item
        if (hdr_succesr(rightHeader) == heapsize) {
            hdr_succesr(hdr_previus(rightHeader)) = heapsize;
        }
        
        if (rightHeader == anchor.succesr) {
            hdr_previus(hdr_succesr(anchor.succesr)) = aaHeader;
            hdr_previus(anchor.succesr) = 0;
            hdr_succesr(aaHeader) = hdr_succesr(anchor.succesr); // point to anchor's successor
        } else {
            hdr_previus(anchor.succesr) = aaHeader; // if anchor doesn't originally point to the rightheader
            hdr_succesr(aaHeader) = anchor.succesr; // point to anchor's successor
        }

       
        
        hdr_previus(aaHeader) = -1; // become the first item of the list
        
        anchor.succesr = aaHeader;
        
        hdr_succesr(rightHeader) = 0;
        hdr_previus(rightHeader) = 0;
    }
    // case 5: FAA -> FA
    else if (hdr_freeall(rightHeader) == 1) {
        hdr_payload(leftHeader) += 2 * sizehdr + hdr_payload(aaHeader);
        hdr_payload(leftFooter) = 0;
        hdr_payload(aaHeader) = 0;
        hdr_payload(aaFooter) = hdr_payload(leftHeader);
        
        
        // if the block to be freed is the last one, update the last item
        if (hdr_succesr(leftHeader) == heapsize) {
            //hdr_previus(hdr_succesr(leftHeader)) = heapsize;
            hdr_succesr(hdr_previus(leftHeader)) = heapsize;
        }
        
       // printf("hdr_previus: %d\n", hdr_previus(leftHeader));
        
        if (anchor.succesr != leftHeader) {
            hdr_succesr(leftHeader) = anchor.succesr; // point to anchor's successor
            hdr_previus(anchor.succesr) = leftHeader; // if anchor doesn't originally point to the leftheader
        }
//        else {
//            hdr_previus(anchor.succesr) = leftHeader; // if anchor doesn't originally point to the rightheader
//            int runner = hdr_succesr(leftHeader);
//            while (runner != heapsize) {
//                if (runner == rightHeader) {
//                    hdr_succesr(leftHeader) = hdr_succesr(runner); // might consume its sucessor
//                    break;
//                }
//                runner = hdr_succesr(runner);
//            }
//            
//        }
        hdr_previus(leftHeader) = -1; // become the first item of the list
        
        anchor.succesr = leftHeader;
         printf("anchor.succesr: %d\n", hdr_succesr(anchor.succesr));
         printf("anchor.previus: %d\n", hdr_previus(anchor.succesr));
        hdr_succesr(aaHeader) = 0;
        hdr_previus(aaHeader) = 0;
    }
    // case 6: FAF -> F
    else {
        hdr_payload(leftHeader) += 4 * sizehdr + hdr_payload(aaHeader) + hdr_payload(rightHeader);
        hdr_payload(leftFooter) = 0;
        hdr_payload(aaHeader) = 0;
        hdr_payload(aaFooter) = 0;
        hdr_payload(rightHeader) = 0;
        hdr_payload(rightFooter) = hdr_payload(leftHeader);
        
        // if the block to be freed is the last one, update the last item
        if (hdr_succesr(rightHeader) == heapsize) {
            hdr_succesr(hdr_previus(rightHeader)) = heapsize;
        }
        else if (hdr_succesr(leftHeader) == heapsize)
        {
            hdr_succesr(hdr_previus(leftHeader)) = heapsize;
        }
        if (anchor.succesr != leftHeader) {
           // hdr_succesr(leftHeader) = anchor.succesr; // point to anchor's successor
            hdr_previus(anchor.succesr) = leftHeader; // if anchor doesn't originally point to the leftheader
        }
        
        if (rightHeader == anchor.succesr) {
            hdr_previus(hdr_succesr(anchor.succesr)) = leftHeader;
            hdr_previus(anchor.succesr) = 0;
//            hdr_succesr(leftHeader) = hdr_succesr(anchor.succesr); // point to anchor's successor
        }
        else {
            hdr_previus(anchor.succesr) = leftHeader; // if anchor doesn't originally point to the rightheader
            int runner = hdr_succesr(leftHeader);
            while (runner != heapsize) {
                if (runner == rightHeader) {
                    hdr_succesr(leftHeader) = hdr_succesr(runner); // might consume its sucessor
                    break;
                }
                runner = hdr_succesr(runner);
            }
            
        }
        
        hdr_previus(leftHeader) = -1; // become the first item of the list
        anchor.succesr = leftHeader;
        
        hdr_succesr(aaHeader) = 0;
        hdr_previus(aaHeader) = 0;
        hdr_succesr(rightHeader) = 0;
        hdr_previus(rightHeader) = 0;

    }
    

    
    *aa = 0;
}

int lalloc( int amt )   // allocates a block of size int
{
    // lalloc code here
    
    // anchor points to a free list
    
    int firstFit = anchor.succesr;
    int sixteenRemainder = amt % 16;
    int change = 0;
    
    if (sixteenRemainder != 0) {
        // add padding if amt is not 16's mutiples
        amt += (16 - sixteenRemainder);
    }
    
    int ami = heapsize;
    while (hdr_succesr(firstFit) != heapsize) {
        
        if (hdr_payload(firstFit) > amt + 32) {
            break; // found free block with enough space
        }
        
        firstFit = hdr_succesr(firstFit);
    }
    
 
    
    /////////////////////// ALLOCATED BLOCK ///////////////////////////
    
    int originalFooter = firstFit+hdr_payload(firstFit)+sizehdr;
    
    if (hdr_payload(firstFit) > (amt+2*sizehdr)) {
        // if a free block has too much space, split the block
        int newFooter = firstFit+amt+sizehdr;
        int newHeader = firstFit+amt+2*sizehdr;
        
        hdr_payload(newFooter) = amt; // add a footer
        hdr_freeall(newFooter) = 1;
        hdr_succesr(newFooter) = 0; // clear sucessor and prev of allocated blck
        hdr_previus(newFooter) = 0;
        
        hdr_payload(newHeader) = hdr_payload(firstFit) - (amt+2*sizehdr); // add a header
        hdr_freeall(newHeader) = 0;
        hdr_succesr(newHeader) = hdr_succesr(firstFit); // update successor
        hdr_previus(newHeader) = hdr_previus(firstFit); // update previous
        
        hdr_payload(originalFooter) = hdr_payload(firstFit) - (amt+2*sizehdr); // update original footer
        hdr_freeall(originalFooter) = 0;
        
        if (hdr_previus(firstFit) == -1) {
            // the allocated block is the first one, update anchor
            anchor.succesr = firstFit+amt+2*sizehdr;
        }
        else {
            hdr_succesr(hdr_previus(firstFit)) = firstFit+amt+2*sizehdr; // add the newly added block to the list
        }
    }
    else {
        hdr_payload(originalFooter) = amt; // update original footer
        hdr_previus(originalFooter) = 0;
        hdr_succesr(originalFooter) = 0;
        hdr_freeall(originalFooter) = 1; // the block is allocated
        if (hdr_previus(firstFit) == -1) {
            // the allocated block is the first one, update anchor
            anchor.succesr = firstFit+amt+2*sizehdr;
        }
        else {
        hdr_succesr(hdr_previus(firstFit)) = hdr_succesr((hdr_succesr(firstFit))); // delete the allocated block and connect it to its successor
        }
    }
    
    hdr_payload(firstFit) = amt; // header
    hdr_freeall(firstFit) = 1; // the block is allocated
    hdr_previus(firstFit) = 0;
    hdr_succesr(firstFit) = 0;
    ///////////////////// ALLOCATED BLOCK END ///////////////////////////
    
    if (hdr_payload(firstFit) < amt + 32) {
        // not enough size, realloc another HEAPSIZE
        int newHeapSize = hdr_payload(heapsize-sizehdr);
        while (newHeapSize < amt + 32) {
            newHeapSize += HEAPSIZE;
            heapsize += HEAPSIZE;
        }
        heap = (char*) realloc(heap, heapsize);
        int newlyAddHead = heapsize;
        int newlyAddFoot = heapsize+HEAPSIZE - sizehdr;
        hdr_freeall(ami) = 1; // pretend the newly added block is allocated
        hdr_payload(ami) = heapsize - ami - 2 * sizehdr;
        hdr_previus(ami) = 0;
        hdr_succesr(ami) = 0;
        hdr_freeall(heapsize - sizehdr) = 1;
        hdr_payload(heapsize - sizehdr) = heapsize - ami - 2 * sizehdr;
        hdr_previus(heapsize - sizehdr) = 0;
        hdr_succesr(heapsize - sizehdr) = 0;
        printf("newlyAddHead: %d\n", newlyAddHead);
        printf("payload newly add head: %d\n", hdr_payload(heapsize));
        
        // find the last block
     
        
        
        // int *location = malloc(1);
        // *location = newlyAddHead + sizehdr;
        //heapsize += HEAPSIZE;
        hdr_succesr(firstFit) = heapsize;
        
        int aii = ami + sizehdr;
        ///lfree(&aii);
        
        firstFit = anchor.succesr;
        change = 1;
        printf("firstFit: %d\n", firstFit);
        printf("hdr_succesr: %d\n", hdr_succesr(anchor.succesr));
        printf("hdr_previus: %d\n", hdr_previus(firstFit));
        
        
        
        return firstFit + sizehdr;
    }

    
//    if (change) {
//        hdr_previus(firstFit) = -1;
//        hdr_succesr(firstFit) = heapsize;
//        hdr_freeall(firstFit) = 0;
//        hdr_freeall(firstFit + 2 * sizehdr + hdr_payload(firstFit)) = 0;
//    }
    
//    heapsize -= (amt + 2*sizehdr); // update heapsize
    //printf("heapsize: %d\n", heapsize);
    return firstFit+sizehdr;
}


// 1. check prev 2. if not free, add to free list 3. check nxt

int main()
{
    int i ;
    int j = 0  ;
    int blks[100] ;
    
    init_heap() ;
    dump_heap( "init heap" ) ;
    
//    lalloc(16);
//    lalloc(16);
//    dump_heap("First allocs");
    for( i=0; i<20; i++ )
        blks[j++] = lalloc( rand()%500 ) ;
    dump_heap( "first allocs" ) ;


//    lfree( &blks[10] ) ;
//    //dump_heap( "AAA -> AFA" ) ;
//    lfree( &blks[11] ) ;
//    dump_heap( "coalesce with upper" ) ;

    lfree( &blks[14] ) ;
    dump_heap( "AAA -> AFA" ) ;
    lfree( &blks[13] ) ;
    dump_heap( "coalesce with lower" ) ;

    lfree( &blks[5] ) ;
    //dump_heap( "AAA -> AFA" ) ;
    lfree( &blks[7] ) ;
    //dump_heap( "AAA -> AFA" ) ;
    lfree( &blks[6] ) ;
    dump_heap( "coalesce with both" ) ;
    
    blks[5] = lalloc( 80000 ) ;
    dump_heap( "blew the top off" ) ;
    /*
    for( i=0; i<5; i++ )
        if (blks[i] != 0 )
        {
            lfree( &blks[i] ) ;
        }
    dump_heap( "free everything " ) ;
    */

    return 0 ;
}