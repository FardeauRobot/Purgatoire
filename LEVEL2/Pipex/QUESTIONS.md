# PIPEX: Concept Mastery Questionnaire

Use this file to test your understanding of the core concepts before or after coding Pipex. Write your answers in the spaces provided.

---

## I. Processes & Execution (`fork`, `execve`, `wait`)

**Q1. What specific resource does `fork()` duplicate, and what does it *not* duplicate?**  
*(Hint: Think about memory, file descriptors, PIDs)*

**Answer:**
<!-- Write your answer here -->



**Q2. Why is `execve` often called a "Brain Transfer"? What happens to the memory of the process that calls it?**

**Answer:**
<!-- Write your answer here -->



**Q3. If `execve` is successful, what line of code executes next in that function?**

**Answer:**
<!-- Write your answer here -->



**Q4. What is a "Zombie Process"? Precisely what system call prevents them?**

**Answer:**
<!-- Write your answer here -->



---

## II. File Descriptors & Pipes (`dup2`, `pipe`)

**Q5. In the context of `dup2(oldfd, newfd)`, if `newfd` was already open (e.g., pointing to a file), what happens to it before the duplication?**

**Answer:**
<!-- Write your answer here -->



**Q6. A pipe created with `pipe(fd)` has two ends. Which index (`fd[0]` or `fd[1]`) is for reading, and which is for writing?**

**Answer:**
<!-- Write your answer here -->



**Q7. Explain why the Parent process must close the write-end of the pipe after forking the children. What specific condition does the reading Child wait for?**

**Answer:**
<!-- Write your answer here -->



---

## III. System Architecture

**Q8. Does a pipe exist on the Hard Disk or in RAM?**

**Answer:**
<!-- Write your answer here -->



**Q9. If a process tries to write to a pipe that is full (typically 64KB), what does the kernel make the process do?**

**Answer:**
<!-- Write your answer here -->



**Q10. How does the shell (and your program) know that `ls` actually means `/usr/bin/ls`? Explain the steps.**

**Answer:**
<!-- Write your answer here -->



---

## IV. Debugging Scenarios

**Q11. You run your pipex, and it hangs forever (doesn't exit). What is the #1 most likely cause involving file descriptors?**

**Answer:**
<!-- Write your answer here -->



**Q12. Your program prints the output of the second command, but it's empty. The first command worked fine. What happened to the data in the pipe?**

**Answer:**
<!-- Write your answer here -->



