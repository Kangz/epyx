/**
 * Epyx exceptions
 */
#ifndef EPYX_ROUTINE_H
#define EPYX_ROUTINE_H

namespace Epyx{

/*
 * A Routine is basically a function that can yield
 * Every Routine you create shoud be cast to the Routine type (it is safe)
 */
struct Routine{
	void (*_rt_call)(void*rt_this);
	void* rt_this;

	/*
	 * call() makes the actual call to the function (you can use the () operator too
	 */
	void call(){
		_rt_call(rt_this);
	}
	void operator()(){
		this->call();
	};
};


//TODO: Add a way to free allocated variables


/*
 * ROUTINE_DECLARE(name, parameter_list) Declares a new routine type whose constructor
 * Takes as parameters the ones given in the list
 * It should be followed by a ROUTINE_LOCAL_VARS
 * See the example in ****.cpp
 */
#define ROUTINE_DECLARE(name, ...) \
	void _rt_call_##name(void *r);\
	\
	struct name{\
		void (*_rt_call)(void* rt_this);\
		void *_rt_this;\
		\
		int _rt_p;\
		void _rt_do();\
		name(__VA_ARGS__);\
		\
		static void _rt_cast_and_call(void *r){\
			name * routine = (name *) r;\
			routine->_rt_do();\
		}\
	
/*
 * ROUTINE_LOCAL_VARS has to be put before the list of local vars
 * It does nothing, really except that it reminds you that local
 * vars are slightly different in routines
 */
#define ROUTINE_LOCAL_VARS

/*
 * ROUTINE_DECLARE_END ands the declaration of a routine
 */
#define ROUTINE_DECLARE_END };



/*
 * ROUTINE_INIT(name, parameter_list) starts the definition of the initializer of the routine
 * which takes as parameters the parameter list.
 * Remember that these parameters are only accessible from ROUTINE_INIT and NOT in ROUTINE_MAIN
 */
#define ROUTINE_INIT(name, ...)\
	name::name(__VA_ARGS__){\
		_rt_p = 0;\
		_rt_this = this;\
		_rt_call = & name ::_rt_cast_and_call;\

/*
 * ROUTINE_INIT_END does what it says
 */
#define ROUTINE_INIT_END }



/*
 * ROUTINE_MAIN(name) starts the definition of the Routine main procedure
 * in which you should NOT use switch ... case statements
 */
#define ROUTINE_MAIN(name)\
	void name::_rt_do(){\
		const int _rt_counter_start = __COUNTER__ + 1;\
		switch(_rt_p){\
		case __COUNTER__ - _rt_counter_start:\

/*
 * ROUTINE_MAIN_END does what it says
 */
#define ROUTINE_MAIN_END }}


/*
 * ROUTINE_YIELD acts as a yield statement in other langages : it jumps back to the caller function
 * yet it is just paused : if you call the routine another time it will resume from exactly that point.
 * ROUTINE_YIELD CANNOT give back a result. If you which to do that use pointers taken as arguments
 * at the initialization
 */
#define _ROUTINE_YIELD(counter) _rt_p=(counter);return;case (counter):1;
#define ROUTINE_YIELD _ROUTINE_YIELD(__COUNTER__ - _rt_counter_start)


}

#endif /* EPYX_ROUTINE_H */
