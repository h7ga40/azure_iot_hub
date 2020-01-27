namespace MicroServer.Net.Http.Mvc
{
	public interface IController
	{
		void OnActionExecuting(ActionExecutingContext context);
		void OnActionExecuted(ActionExecutedContext context);
		void OnException(ExceptionContext context);
	}
}
