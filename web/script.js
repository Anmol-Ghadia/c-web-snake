document.addEventListener('click', function(event) {
    event.preventDefault();

    give_touch_input(event.clientX,event.clientY);
});

document.addEventListener('touchstart', function(event) {
    event.preventDefault();
    
    var touch = event.touches[0];
    give_touch_input(touch.clientX,touch.clientY);
});
