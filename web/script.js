document.addEventListener('pointerdown',function(event) {
    event.preventDefault();

    give_touch_input(event.clientX,event.clientY);
})

document.addEventListener('keydown',function(event) {
    switch (event.key) {
        case "w":
        case "ArrowUp":
            give_key_input(0);
            break;
        case "d":
        case "ArrowRight":
            give_key_input(1);
            break;
        case "s":
        case "ArrowDown":
            give_key_input(2);
            break;
        case "a":
        case "ArrowLeft":
            give_key_input(3);
            break;
        default:
            break;
    }
})
