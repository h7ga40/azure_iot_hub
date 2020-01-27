namespace MicroServer.Net.Http.Mvc
{
	public abstract class ActionResult : IActionResult
	{
		public abstract void ExecuteResult(IControllerContext context);
	}
}
