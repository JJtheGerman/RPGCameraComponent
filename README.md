# RPGCameraComponent
A camera component mimicking basic behavior of most RPGs or games like Foxhole.  
*Mainly a test in completely encapsulating a component by letting it handle input and sub component spawning itself.*  

---

![1](Resources/RPGComponentPreview.gif)  

---

#### Why?
I noticed a while ago that the bigger projects get the more messy the character class becomes. Despite using components  
the character will baloon to thousands of lines of entangled code.  
This component is different in that you simply add it to your ACharacter, set a few **UPROPERTIES** and you are done.  
No further interaction with the component needed to make it work.   
*You do not even see the components it creates in the outliner of the character which I have yet to decide wether or not its a feature.*

--- 

#### How to make it work
1. Add the component onto a class derived from ACharacter
2. Bind Action Input "RotateCamera" in your Project Settings
3. Bind Axis Input "MouseScroll" in your Project Settings
