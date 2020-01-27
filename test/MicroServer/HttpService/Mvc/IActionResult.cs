namespace MicroServer.Net.Http.Mvc
{
	public interface IActionResult
	{
		void ExecuteResult(IControllerContext context);
	}
}
