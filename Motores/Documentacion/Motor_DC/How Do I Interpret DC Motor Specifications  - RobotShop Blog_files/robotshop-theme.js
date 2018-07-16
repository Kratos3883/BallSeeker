/*!
 * Robotshop Theme
 * Manages Wordpress theme user experience
 */
var robotshopTheme = {};

robotshopTheme.jQuery = window.jQuery.noConflict();
robotshopTheme.$window   = robotshopTheme.jQuery(window);
robotshopTheme.window   = window;
robotshopTheme.$document = robotshopTheme.jQuery(document);
robotshopTheme.document = document;
robotshopTheme.$document.ready(function(){
    robotshopTheme.$html = robotshopTheme.jQuery('html');
    robotshopTheme.$body = robotshopTheme.jQuery('body');
});

/**
 * Helpers methods exposed to the global scope
 *
 * Use example:
 * robotshopTheme.helpers.isIE()
 */
robotshopTheme.helpers = (function(window, document, $) {

    var public  = {},
        local   = {},
        helpers = {};

    /**
     * Returns current breakpoint label
     */
    public.getDevice = function() {
        var width = public.getTrueWidth();
        var device = "";

        if     (width > 1699) { device = 'xxl-desktop'; }
        else if(width > 1528) { device = 'xl-desktop'; }
        else if(width > 1200) { device = 'l-desktop'; }
        else if(width > 1023) { device = 'desktop'; }
        else if(width > 767)  { device = 'tablet'; }
        else if(width > 479)  { device = 'l-mobile'; }
        else                  { device = 'mobile' }

        return device;
    }

    /**
     * Responsive helpers
     */
    public.isMobile = function() {
        var device = public.getDevice();
        if(device == 'mobile' || device == 'l-mobile' || device == 'tablet') {
            return true;
        }
        return false;
    }
    public.isIE8 = function() {
        if(robotshopTheme.$body.hasClass('ie8') || robotshopTheme.$html.hasClass('ie8')) {
            return true;
        }
        return false;
    }
    public.isIE = function() {
        if(robotshopTheme.$body.hasClass('ie') || robotshopTheme.$html.hasClass('ie')) {
            return true;
        }
        return false;
    }
    public.isDesktop = function() {
        var device = public.getDevice();
        if(device == 'desktop' || device == 'l-desktop' || device == 'xl-desktop' || device == 'xxl-desktop') {
            return true;
        }
        return false;
    }

    /**
     * Checks if body has css class, returns bool
     */
    public.bodyHasClass = function(selector) {
        return robotshopTheme.$body.hasClass(selector);
    }

    /**
     * Retrieves the viewport offset of a jQuery dom element
     */
    public.getViewportOffset = function($e) {
        var $window = robotshopTheme.$window,
        scrollLeft = $window.scrollLeft(),
        scrollTop = $window.scrollTop(),
        offset = $e.offset(),
        rect1 = { x1: scrollLeft, y1: scrollTop, x2: scrollLeft + $window.width(), y2: scrollTop + $window.height() },
        rect2 = { x1: offset.left, y1: offset.top, x2: offset.left + $e.width(), y2: offset.top + $e.height() };
        return {
            top: offset.top - scrollTop,
            right: offset.top - scrollTop + $e.outerWidth(),
            bottom: offset.top - scrollTop + $e.outerHeight(),
            left: offset.left - scrollLeft,
            insideViewport: rect1.x1 < rect2.x2 && rect1.x2 > rect2.x1 && rect1.y1 < rect2.y2 && rect1.y2 > rect2.y1
        };
    }

    /**
     * Equalize height of a list of dom elements by selectors.
     * Set the css height of the tallest element to the matched dom elements.
     */
    public.heightsEqualizer = function(selector) {
        var elements = document.querySelectorAll(selector),
            max_height = 0,
            len = 0,
            i;
        if ( (elements) && (elements.length > 0) ) {
            len = elements.length;

            for (i = 0; i < len; i++) { // get max height
                elements[i].style.height = ''; // reset height attr
                if (elements[i].clientHeight > max_height) {
                    max_height = elements[i].clientHeight;
                }
            }

            for (i = 0; i < len; i++) { // set max height to all elements
                if(max_height == 0) {
                    elements[i].style.height = 'inherit';
                }else {
                    elements[i].style.height = max_height + 'px';
                }
            }
        }
    };

    /**
     * Simple throttle to avoid choking the site with scroll/resize event watcher
     */
    public.throttle = function(callback, limit) {
        var wait = false;
        return function () {
            if (!wait) {
                callback.call();
                wait = true;
                setTimeout(function () {
                    wait = false;
                }, limit);
            }
        }
    };

    /**
     * Legacy code to get website true width
     */
    public.getTrueWidth = function() {
        return ( robotshopTheme.$window.width() + public.getScrollBarWidth() );
    }
    public.getScrollBarWidth = function() {
        if(robotshopTheme.$document.height() > robotshopTheme.$window.height()){
            robotshopTheme.$body.append('<div id="fakescrollbar" style="width:50px;height:50px;overflow:hidden;position:absolute;top:-200px;left:-200px;"></div>');
            fakeScrollBar = $('#fakescrollbar');
            fakeScrollBar.append('<div style="height:100px;">&nbsp;</div>');
            var w1 = fakeScrollBar.find('div').innerWidth();
            fakeScrollBar.css('overflow-y', 'scroll');
            var w2 = $('#fakescrollbar').find('div').html('html is required to init new width.').innerWidth();
            fakeScrollBar.remove();

            return (w1-w2);
        }
    };

    return public;
})(robotshopTheme.window, robotshopTheme.document, robotshopTheme.jQuery);

/**
 * Manages the main menu
 * Legacy code taken as is and only slightly refactored
 */
robotshopTheme.menu = function(context) {

    /* Grab this to debug: */
    /* jQuery('.dropdown-menu').first().addClass('current-open').show().find('.dropdown-submenu').first().children('.dropdown-menu').addClass('current-open').show().find('.dropdown-submenu').first().children('.dropdown-menu').addClass('current-open').show(); */
    var window = context.window;
    var document = context.document;
    var $ =  context.$;

    var public  = {};
    var local   = {};
    var helpers = {};

    local.checkResizeWidth = 0;

    var manageMenuClass = function() {
        $('.mainTopMenuDesktop .dropdown-submenu, .mainTopMenuDesktop li').die('mouseenter mouseleave');
        $('.header-container .headerMenuIcon, .mainTopMenuMobile li.level0 .dropdown-toggle, .mainTopMenuMobile li.level1.dropdown-submenu .MobileToggle, .mainTopMenuMobile li.level2.dropdown-submenu .MobileToggle').die('click');
        $('.Menu > ul.mainTopMenuMobile').removeClass('mainTopMenuMobile');
        $('.Menu > ul').addClass('mainTopMenuDesktop');
    }

    var openDesktopMenu = function(row) {
        $(row).find('.dropdown-menu').first().show().addClass('current-open');
        if($('.current-open').length > 1){
            robotshopTheme.helpers.heightsEqualizer('.current-open');
        }
    }

    var closeDesktopMenu = function(row) {
        $(row).find('.current-open').css({height:'auto'}).hide().removeClass('current-open');
        $(row).closest('.current-open').css({height:'auto'})
        if($('.current-open').length > 1){
            robotshopTheme.helpers.heightsEqualizer('.current-open');
        }
    }

    /*Used by menuAim*/
    var exitDesktopMenu = function() {
        var doDeactivationEvent = true;
        return doDeactivationEvent;
    }

    var setTopMenu = function() {
        $('.mainTopMenuDesktop li.level0').live({
            mouseenter: function() {
                var $elem = $(this).find('.dropdown-menu').first();
                $elem.show().addClass('current-open');
                if(robotshopTheme.helpers.isIE8()) {
                    /* force redraw */
                    $(this).height($(this).height()+2);
                    $(this).height($(this).height()-1);$(this).height($(this).height()-1);
                }
            },

            mouseleave: function () {
                $(this).find('.dropdown-menu').first().hide().removeClass('current-open');
                if(robotshopTheme.helpers.isIE8()) {
                    /* force redraw */
                    $(this).height($(this).height()+2);
                    $(this).height($(this).height()-1);$(this).height($(this).height()-1);
                }
            }
        });

        var menuAimOptions = {
            activate: openDesktopMenu,
            deactivate: closeDesktopMenu,
            submenuSelector: ".dropdown-submenu",
            exitMenu: exitDesktopMenu,
            rowSelector: "> li"
        };
        if(robotshopTheme.helpers.isIE8()) {
            menuAimOptions.tolerance = 100;
        }

        $(".mainTopMenuDesktop .level0 .dropdown-menu").menuAim(menuAimOptions);

        // Mobile
        $('.header-container .headerMenuIcon').live('click',function(e){
            $('.mainTopMenuMobile').find('.dropdown-menu').css({height:'auto'}).removeClass('current-open');
            if ( $(this).hasClass('activeNav') ) {
                $(this).removeClass('activeNav');
                $(this).next().hide();
            } else {
                $(this).addClass('activeNav');
                $(this).next().show();

                $('.top-cart .block-title.expanded').removeClass('expanded');
                $('.top-cart .block-content').slideUp();
            }
        });

        $('.top-cart .icon-shopping-cart').live('click',function(e){
            if ( robotshopTheme.helpers.getTrueWidth() < 1024 && !robotshopTheme.$html.hasClass('ie8') ) {
                $( '.headerMenuIcon.activeNav' ).trigger( "click" );
            }
        });

        $('.mainTopMenuMobile li.level0 .dropdown-toggle').live('click',function(e){
            $('.mainTopMenuMobile').find('.dropdown-menu').css({height:'auto'}).removeClass('current-open');
            if ( $(this).parent().hasClass('activeNav') ) {
                $('.nav li.level0.activeNav').removeClass('activeNav');
            } else {
                $('.nav li.level0.activeNav, .nav li.level1.activeNav, .MobileToggle.activeNav').removeClass('activeNav');
                $(this).parent().addClass('activeNav');

                $('html, body').animate({scrollTop: $(this).offset().top}, 500);
            }

            e.stopImmediatePropagation();
        });

        $('.mainTopMenuMobile li.level1.dropdown-submenu .MobileToggle a').live('click',function(){
            if ( $(this).parent().parent().hasClass('activeNav') ) {
                $(this).parents("li:eq(0)").removeClass('activeNav');
            } else {
                $(this).parent().parent().siblings('li.level1.activeNav').removeClass('activeNav');
                $(this).parent().parent().addClass('activeNav');

                $('html, body').animate({scrollTop: $(this).offset().top}, 500);
            }
        });

        $('.mainTopMenuMobile li.level2.dropdown-submenu .MobileToggle a').live('click',function(e){
            if ( $(this).parent().hasClass('activeNav') ) {
                $('.nav li.level2 .activeNav').removeClass('activeNav');
            } else {
                $('.nav li.level2 .activeNav').removeClass('activeNav');
                $(this).parent().addClass('activeNav');
                $('html, body').animate({scrollTop: $(this).offset().top}, 500);
            }
        });

        $('#topCartBlock').click(function() {
            if ( robotshopTheme.helpers.getTrueWidth() < 1024 && !robotshopTheme.$html.hasClass('ie8') && !robotshopTheme.$body.hasClass('ie8') ) {
                if ( $( '#topCartBlock .block-title' ).hasClass( 'expanded' ) ) {
                    $('html, body').animate({scrollTop: $( '.TopHeaderPanel' ).offset().top + 66 }, 500);
                }
            }
        });
    }

    var checkResize = function() {

        if ( robotshopTheme.helpers.getTrueWidth() == local.checkResizeWidth ){
            return;
        }

        manageMenuClass();

        // Clear display when resize screen
        $('.header-container .Menu .nav, .Menu .dropdown-menu, ul.nav li.level2').css('display', '');
        $('ul.nav li.level2.dropdown-submenu a').next().css('display', '');

        // Clear active state when resize screen
        $('.header-container .headerMenuIcon, .nav li.level0.activeNav').removeClass('activeNav');
        $('.MobileToggle').removeClass('activeNav');

        local.checkResizeWidth = robotshopTheme.helpers.getTrueWidth();
    };

    var initMainMenu = function() {
        setTopMenu();
        manageMenuClass();
        local.checkResizeWidth = robotshopTheme.helpers.getTrueWidth();
        if(!robotshopTheme.$html.hasClass('ie8')) {
            robotshopTheme.$window.on("resize", robotshopTheme.helpers.throttle(checkResize, 10));
        }
    };

    var init = function() {
        initMainMenu();
    };

    robotshopTheme.$document.ready(function(){
        init();
    });

    return public;
};

/**
 * App entry point
 * Does "routing" to trigger correct modules and explose public namespaces to global
 *
 * Use example:
 * robotshopTheme.app.currentPage.myMethod()
 */
robotshopTheme.app = (function(window, document, $) {

    appContext = {
        'window':window,
        'document':document,
        '$':$
    };

    var public  = {},
        local   = {},
        helpers = {};

    var init = function() {
        public.menu = robotshopTheme.menu(appContext);
    }
    robotshopTheme.$document.ready(function(){
        init();
    });
    robotshopTheme.$window.on("load", function(){
        robotshopTheme.$window.trigger('resize'); /* Recalculate responsive when all image are loaded */
    });

    return public;
})(robotshopTheme.window, robotshopTheme.document, robotshopTheme.jQuery);





