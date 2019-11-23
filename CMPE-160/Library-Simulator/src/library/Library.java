package library;

import java.util.Scanner;
import books.Book;
import books.Handwritten;
import books.Printed;
import librarymembers.Academic;
import librarymembers.LibraryMember;
import librarymembers.Student;

/**
 * 
 * @author ZuhalDidemAytac
 * where all validity checks are done and the proper methods are called if the action is valid.
 * books is an array of size 10^6 and holds the Book objects that are created.
 * members is an array of size 10^6 and holds the LibraryMember objects that are created.
 * totalFee is a static integer value and holds the total penalties that have been paid to the library by members.
 * currBookType and currMemType are string values read by the scanner, specifying the type of books and members.
 * currMem and currBook are LibraryMember and Book objects we are currently making operations with.
 * currMemID and currBookID are id numbers of LibraryMember and Book objects we are currently making operations with.
 * bookIDs and memIDs hold one more of the current book and member numbers. The values are assigned to newly created objects and than these values are incremented, they are static.
 * a scanner is defined to read the file. 
 */

public class Library {
	private Book books[] = new Book[1000009];
	private LibraryMember members[] = new LibraryMember[1000009];
	public static int totalFee;
	private String currBookType;
	private String currMemType;
	private LibraryMember currMem;
	private Book currBook;	
	private int currBookID;
	private int currMemID;
	private static int bookIDs = 1;
	private static int memberIDs = 1;
	private Scanner scanner;

	/**
	 * @param scanner of the MAİN?? class is passed as parameter to the constructor.
	 * 
	 */
	public Library(Scanner scanner) {
		this.scanner = scanner;
	}

	/**
	 * is the operation of adding a new book to library.
	 * First we check the number of books, it can at maximum be 999999, so the bookIDs value which is numberOfBooks+1 can at maximum be 1000000.
	 * We first read the bookType with the scanner and define a temporary Book object.
	 * If the bookType is H a new handwritten book object is created and assigned to tempbook.
	 * bookIDs value is incremented because no other book can have the same id.
	 * The same process in last two steps are done if the bookType is P. (A new printed book object is created.)
	 * After both, the new book object is added to the books array to the index of bookIDs-2 (Because we have incremented bookIDs value just before and array indexes start from 0.
	 * Tempbook is assigned to null so as to prevent any errors.
	 * 
	 */
	public void addBook() {
		if(bookIDs<1000000) {
			currBookType = scanner.next();
			Book tempbook;
			if (currBookType.equalsIgnoreCase("H")) {
				tempbook = new Handwritten(bookIDs);
				bookIDs++;
			} else {
				tempbook = new Printed(bookIDs);
				bookIDs++;
			}
			books[bookIDs - 2] = tempbook;
			tempbook = null;
		}
	}		

	/**
	 * the operation of adding a new member to library.
	 * First we check the number of members, it can at maximum be 999999, so the memberIDs value which is numberOfMembers+1 can at maximum be 1000000.
	 * We first read the memberType with the scanner and define a temporary LibraryMember object.
	 * If the memberType is S a new Student object is created and assigned to tempmem.
	 * memberIDs value is incremented because no other member can have the same id.
	 * The same process in last two steps are done if the memberType is A. (A new Academic object is created.)
	 * After both, the new LibraryMember object is added to the members array to the index of memberIDs-2 (Because we have incremented memberIDs value just before and array indexes start from 0.
	 * Tempmem is assigned to null so as to prevent any errors.
	 * 
	 */
	public void addMember() {
		if(memberIDs<1000000){
			currMemType = scanner.next();
			LibraryMember tempmem;
			if (currMemType.equalsIgnoreCase("S")) {
				tempmem = new Student(memberIDs);
				memberIDs++;
			} else {
				tempmem = new Academic(memberIDs);
				memberIDs++;
			}
			members[memberIDs - 2] = tempmem;
			tempmem = null;
		}
	}

	/**
	 * @param tick is the time zone we are currently in.
	 * does the validity checks for a borrowing operation.
	 * We read the IDs of the book and member with the scanner, respectively.
	 * A member ID and a book ID can at most be a 6 digit number,999999. Which means they have to be both smaller than 1000000.
	 * We check the positions of the members with that id numbers in both book and member arrays (which are id-1).
	 * If the objects are not null, we continue with assigning current member and current book objects to the members and arrays we want to make operation with.
	 * Only printed books can be borrowed, so we check if bookType is equal to P.
	 * For a member to borrow a book, it must be available. So we check whether the isTaken field is false.
	 * Every member has a limit for book borrowing and has a field that holds the number of books he/she has borrowed and not returned yet.
	 * The number of books the member has borrowed currently should be smaller than the limit for that member.
	 * And finally, the member should not have an unpaid fee, which means the member should not occupy a book whose deadline is passed and yet not returned.
	 * If the current member and book pass from all these cases, the borrowBook method in Printed class is called.
	 * 
	 */
	public void borrowBook(int tick) {
		currBookID = scanner.nextInt();
		currMemID = scanner.nextInt();
		if (currMemID<1000000 && currBookID<1000000) {
			if (!(members[currMemID - 1] == null) && !(books[currBookID - 1] == null)) {
				currMem = members[currMemID - 1];
				currBook = books[currBookID - 1];
	
				if (currBook.getBookType().equals("P") && !currBook.isTaken()
						&& currMem.getHasNoOfBooks() < currMem.getMaxNumberOfBooks() && !hasUnpaidFee(currMem, tick)) {
					((Printed) currBook).borrowBook(currMem, tick);
	
				}
			}
		}
	}

	/**
	 * @param tick is the time zone we are currently in.
	 * does the validity checks for a returning operation.
	 * We read the IDs of the book and member with the scanner, respectively.
	 * A member ID and a book ID can at most be a 6 digit number,999999. Which means they have to be both smaller than 1000000 
	 * We check the positions of the members with that id numbers in both book and member arrays (which are id-1).
	 * If the objects are not null, we continue with assigning current member and current book objects to the members and arrays we want to make operation with.
	 * For a book to be returned, it must first of all be taken and the whoHas field of that book should be equal to the member that is trying to return it.
	 * Both printed and handwritten books can be returned, and returning operations are different for each of them.
	 * If bookType is equal to P, which means it's a printed book, the whoHas field being equal to the member does not satisfy us because the member can either have borrowed or read that book in library.
	 * If it has been borrowed, the book must be in the booksInHand arrayList of that member. 
	 * (If it is not, that means the member has read that book in library and now returning it, we must not check the deadline for the books read in library.)
	 * In that situation, we check whether the deadline has passed. If it has, the member pays a penalty.
	 * After that we call the returnBook method for printed books.
	 * Handwritten books can be returned too, as they can be read by the academicians in the library.
	 * If bookType is H, we call the returnBook method for handwritten books.
	 */
	public void returnBook(int tick) {

		currBookID = scanner.nextInt();
		currMemID = scanner.nextInt();
		if (currMemID<1000000 && currBookID<1000000) {
			if (!(members[currMemID - 1] == null) && !(books[currBookID - 1] == null)) {
				currMem = members[currMemID - 1];
				currBook = books[currBookID - 1];
				if (currBook.isTaken() && currBook.getWhoHas().equals(currMem)) {
					if (currBook.getBookType().equals("P")) {
						if (currMem.booksInHand.size() > 0 && currMem.booksInHand.contains(currBook)
								&& ((Printed) currBook).getDeadline() < tick) {
							totalFee += tick - ((Printed) currBook).getDeadline();
						}
						((Printed) currBook).returnBook(currMem);
	
					} else {
						((Handwritten) currBook).returnBook(currMem);
					}
	
				}
			}
		}
	}

	/**
	 * @param tick is the time zone we are currently in.
	 * does the validity checks for an extension operation.
	 * We read the IDs of the book and member with the scanner, respectively.
	 * A member ID and a book ID can at most be a 6 digit number,999999. Which means they have to be both smaller than 1000000
	 * We check the positions of the members with that id numbers in both book and member arrays (which are id-1).
	 * If the objects are not null, we continue with assigning current member and current book objects to the members and arrays we want to make operation with.
	 * For a book to be extended, it must first of all be taken and the whoHas field of that book should be equal to the member that is trying to extend it.
	 * Only printed books can be borrowed and has a deadline which means only printed books can be extended.
	 * Every member can extend the deadline of a specific book only once for each borrowing operation.
	 * We check whether the member has extended the book for the current borrowing operation before.
	 * And finally, the deadline should not have passed for an extension to be done.
	 * If all of these are satisfied extend method in Printed class is called.
	 */
	public void extendBook(int tick) {

		currBookID = scanner.nextInt();
		currMemID = scanner.nextInt();
		if (currMemID<1000000 && currBookID<1000000) {
			if (!(members[currMemID - 1] == null) && !(books[currBookID - 1] == null)) {
				currMem = members[currMemID - 1];
				currBook = books[currBookID - 1];
	
				if (currBook.isTaken() && currBook.getWhoHas().equals(currMem) && !((Printed) currBook).isExtended()
						&& ((Printed) currBook).getDeadline() >= tick) {
					((Printed) currBook).extend(currMem, tick);
	
				}
			}
		}
	}

	/**
	 * does the validity checks for a reading operation.
	 * We read the IDs of the book and member with the scanner, respectively.
	 * A member ID and a book ID can at most be a 6 digit number,999999. Which means they have to be both smaller than 1000000
	 * We check the positions of the members with that id numbers in both book and member arrays (which are id-1).
	 * If the objects are not null, we continue with assigning current member and current book objects to the members and arrays we want to make operation with.
	 * For a member to read a book, it must be available. So we check whether the isTaken field is false.
	 * Printed books are available for every one to read, however if the book is handwritten, only Academic members can read it.
	 * If all of these are satisfied, readBook method is called for that book.
	 * 
	 */
	public void readInLibrary() {
		currBookID = scanner.nextInt();
		currMemID = scanner.nextInt();
		if (currMemID<1000000 && currBookID<1000000) {
			if (!(members[currMemID - 1] == null) && !(books[currBookID - 1] == null)) {
				currMem = members[currMemID - 1];
				currBook = books[currBookID - 1];
	
				if (!currBook.isTaken()) {
					if (currBook.getBookType().equals("P")
							|| (currBook.getBookType().equals("H") && currMem instanceof Academic)) {
						currBook.readBook(currMem);
					}
	
				}
			}
		}
	}

	/**
	 * @param mem is the LibraryMember we are investigating whether has an unpaid fee or not
	 * @param tick is the time zone we are currently in.
	 * @return true if the member has an unpaid fee, false if not.
	 * booksInHand is the ArrayList that holds the books the member has borrowed and not returned back yet.
	 * So we travel the array and check the deadline for every book object in that array.
	 * If deadline is smaller than tick, that means the deadline has passed, so the method returns true and makes it impossible for the member to borrow a new book.
	 * After traveling the whole array, method returns false if a non paid fee is not found for that member.
	 */
	public boolean hasUnpaidFee(LibraryMember mem, int tick) {
		for (int i = 0; i < mem.booksInHand.size(); i++) {
			if ((mem.booksInHand.get(i)).getDeadline() < tick) {
				return true;
			}
		}
		return false;
	}

	/**
	 * @return all Book objects registered to the library system.
	 */
	public Book[] getBooks() {
		return books;
	}

	/**
	 * @return all LibraryMember objects registered to the library system.
	 */
	public LibraryMember[] getMembers() {
		return members;
	}

	/**
	 * @return the total fee paid to the Library by members who returned a book after it's deadline.
	 */
	public static int getTotalFee() {
		return totalFee;
	}

}